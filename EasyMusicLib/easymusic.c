#include "easymusic.h"

static MCIERROR last_error;

//----------------通用----------------
//获取最近一次调用时的 MCI 错误码。
MCIERROR em_get_last_error()
{
	return last_error;
}

//获取指定 MCI 错误码对应的错误描述。
char* em_get_error(MCIERROR error_code) 
{
	char* str = malloc(sizeof(char) * 1024);
	bool result = (bool)mciGetErrorStringA(error_code, str, 1024);
	return result ? str : NULL;
}

//按“分分:秒秒”的格式输出时间
void em_format_time(double sec, char *output)
{
	int mm = (int)(sec / 60);
	int ss = (int)sec % 60;
	sprintf(output, "%02d:%02d", mm, ss);
}

//----------------歌词----------------

//为 Lyric 结构体分配空间并解析 .lrc 歌词文本
static EM_ERR lyric_init(Lyric *lyric, const char *str)
{
	if (str == NULL) //参数检查勿漏
		return EM_ERR_PARAM_INVAILD;
	//首先估计歌词的行数，以分配空间
	//通过换行的数量来估计歌词行数
	const char *ptr = str; //输入字符串的指针
	int count = 0; // \n 即换行符的数量
	while (ptr != NULL && ptr >= str && ptr - str < (int)strlen(str))
	{
		ptr = strchr(ptr, '\n');
		count++;
		ptr++;
	}
	
	//分配空间
	lyric->length = count;
	lyric->time_array = calloc(sizeof(double), count);
	lyric->lyric_array = calloc(sizeof(char*), count);

	//.lrc 歌词每一行的格式都如下：[03:01.99]xxxx
	//开头的时间可能出现多次：[00:01.80][00:04.92][01:18.27][01:20.97][03:15.35][03:18.30]xxxx
	//TODO 支持多个时间

	//由于 strtok 会改变原字符串，所以需要先复制备份一份
	char *str_new = malloc(strlen(str) + 1);
	if (str_new == NULL)
		return EM_ERR_ALLOC_FAILED;
	strcpy(str_new, str);

	//开始对每一行进行分析
	char *line = strtok(str_new, "\n"); //按 \n 分割字符串
	int lyric_line_index = 0;
	while (line != NULL)
	{
		int index = 0;
		if (line[index] != '[') //不是以 [ 开头的忽略掉
			goto next;
		while (line[index] != ']')
		{
			index++;
			if (index > strlen(line))
				goto next;
		}
		
		//提取出时间并转换为秒
		int mm = 0;
		double ss = 0;
		sscanf(line, "[%d:%lf]", &mm, &ss);
		lyric->time_array[lyric_line_index] = mm * 60 + ss; //储存该行时间

		//提取出歌词
		char *text_line = line + index + 1;
		lyric->lyric_array[lyric_line_index] = malloc(strlen(text_line) + 2);
		if (lyric->lyric_array[lyric_line_index] == NULL)
			return EM_ERR_ALLOC_FAILED;
		strcpy(lyric->lyric_array[lyric_line_index], text_line);
		lyric_line_index++;
		if (lyric_line_index > lyric->length)
			system("pause");
	next:
		line = strtok(NULL, "\n"); //按行分割字符串
		
	}
	free(str_new);

	return EM_ERR_OK;
}

//从字符串中创建 Lyric 结构体
Lyric *lyric_create_from_string(const char *str)
{
	Lyric *lyric = malloc(sizeof(Lyric));
	memset(lyric, 0, sizeof(Lyric));
	if (lyric == NULL)
		return NULL;
	lyric_init(lyric, str);
	return lyric;
}

//获取指定行的歌词
EM_ERR lyric_get(Lyric *lyric, int index, char **current_line)
{
	if (index < 0 || index > lyric->length)
		return EM_ERR_PARAM_INVAILD;

	*current_line = lyric->lyric_array[index];
	return EM_ERR_OK;
}

//更新歌词，获取当前歌词的下标
EM_ERR lyric_update_index(Lyric *lyric, double position, int *current_line_index)
{
	//思路：遍历所有歌词，找到离当前时间点最近且不超过当前时间的歌词行
	double min_delta_time = 999999.0;
	for (size_t i = 0; i < lyric->length; i++)
	{
		double result = position - lyric->time_array[i];
		if (result > 0 && result < min_delta_time)
		{
			min_delta_time = result;
			*current_line_index =i; //把时间差最小的歌词行的下标保存下来
		}
	}

	return min_delta_time != 999999.0; //如果 min_delta_time 还是 999999.0 说没找到
}

//更新歌词，获取当前歌词文本
EM_ERR lyric_update(Lyric *lyric, double position, char **current_line)
{
	int index = 0;
	EM_ERR err = 0;
	err = lyric_update_index(lyric, position, &index);
	if (err != EM_ERR_OK)
		return err;

	return lyric_get(lyric, index, current_line);
}

//----------------播放器----------------

MusicPlayer *player_create()
{
	MusicPlayer *player = malloc(sizeof(MusicPlayer));
	if (player == NULL)
		return NULL;
	memset(player, 0, sizeof(MusicPlayer));
	return player;
}

//打开一个文件。
//	参数：
//		player -> EasyPlayer 结构体的指针
//		file_path -> 文件路径
//	返回值：
//		错误码，即 EM_ERR_ 开头的常量。
EM_ERR player_open(MusicPlayer *player, const char *file_path)
{
	MCI_OPEN_PARMS params;
	params.lpstrDeviceType = NULL; //设备类型为空，表示自动推断
	params.lpstrElementName = (LPCWSTR)file_path;
	last_error = mciSendCommandA((MCIDEVICEID)NULL, MCI_OPEN, MCI_OPEN_ELEMENT, (DWORD)(LPVOID)&params);
	if (last_error != 0)
		return EM_ERR_FAILED_TO_LOAD;
	player->device_id = params.wDeviceID;
	return EM_ERR_OK;
}

//关闭播放器
EM_ERR player_close(MusicPlayer *player)
{
	last_error = mciSendCommand(player->device_id, MCI_CLOSE, 0, NULL);
	return EM_ERR_OK;
}

//播放。
EM_ERR player_play(MusicPlayer *player)
{
	MCI_PLAY_PARMS params = { 0 };
	last_error = mciSendCommandA(player->device_id, MCI_PLAY, (DWORD_PTR)NULL, (DWORD_PTR)&params);
	if (last_error == 0)
		return EM_ERR_OK;
	else
		return EM_ERR_UNKNOWN;
}

//从指定开始位置播放到指定结束位置
EM_ERR player_play_advanced(MusicPlayer *player, double fromSec, double toSec)
{

}

//暂停
EM_ERR player_pause(MusicPlayer *player)
{
	last_error = mciSendCommandA(player->device_id, MCI_PAUSE, 0, (DWORD_PTR)NULL);
	return EM_ERR_OK;
}

//停止
EM_ERR player_stop(MusicPlayer *player)
{
	last_error = mciSendCommandA(player->device_id, MCI_STOP, 0, (DWORD_PTR)NULL);
	return EM_ERR_OK;
}

//相对于当前位置前进/后退。
EM_ERR player_seek_relative(MusicPlayer *player, double relative_sec)
{
	double pos;
	EM_ERR err;
	err = player_position_get(player, &pos);
	if (err != EM_ERR_OK)
		return err;
	player_seek(player, pos + relative_sec);
}

//设置播放位置到指定时间。
EM_ERR player_seek(MusicPlayer *player, double position_sec) 
{
	MCI_SEEK_PARMS mciSeekParms;
	mciSeekParms.dwTo = (DWORD)(position_sec * 1000);
	last_error = mciSendCommandA(player->device_id, MCI_SEEK | MCI_TO, (DWORD)NULL, (DWORD_PTR)&mciSeekParms);
}

//查询当前播放状态。
EM_ERR player_status(MusicPlayer *player, PLAYER_STATUS *status)
{
	MCI_STATUS_PARMS mciStatusParms = {0};
	mciStatusParms.dwItem = MCI_STATUS_MODE;
	last_error = mciSendCommandA(player->device_id, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID)&mciStatusParms);
	
	switch (mciStatusParms.dwReturn)
	{
	case MCI_MODE_PLAY:
		*status = PLAYER_STATUS_PLAYING;
		break;
	case MCI_MODE_STOP:
		*status = PLAYER_STATUS_STOPPED;
		break;
	case MCI_MODE_PAUSE:
		*status = PLAYER_STATUS_PAUSED;
		break;
	case MCI_MODE_SEEK:
		*status = PLAYER_STATUS_BUFFERING;
		break;
	default:
		*status = PLAYER_STATUS_UNKNOWN;
		break;
	}

	if (last_error == 0)
		return EM_ERR_OK;
	else
		return EM_ERR_UNKNOWN;
}

//获取当前打开的媒体文件的总时长。
EM_ERR player_length_get(MusicPlayer *player, double *seconds)
{
	MCI_STATUS_PARMS params = { 0 };
	params.dwItem = MCI_STATUS_LENGTH;
	last_error = mciSendCommandA(player->device_id, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID)&params);
	*seconds = params.dwReturn / 1000.0;
	return EM_ERR_OK;
}

//获取当前播放的位置。
EM_ERR player_position_get(MusicPlayer *player, double *seconds)
{
	MCI_STATUS_PARMS params = {0};

	params.dwItem = MCI_STATUS_POSITION;
	last_error = mciSendCommandA(player->device_id, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID)&params);
	*seconds = params.dwReturn / 1000.0;
	return EM_ERR_OK;
}

//获取当前打开的媒体文件的总时长。简化版
 double player_length_get_(MusicPlayer *player)
{
	double ret = 0;
	player_length_get(player, &ret);
	return ret;
}

//获取当前播放的位置。简化版
double player_position_get_(MusicPlayer *player)
{
	double ret = 0;
	player_position_get(player, &ret);
	return ret;
}

//
bool player_status_playing_(MusicPlayer *player)
{
	PLAYER_STATUS status = PLAYER_STATUS_UNKNOWN;
	player_status(player, &status);
	return status == PLAYER_STATUS_PLAYING;
}