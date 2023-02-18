#include "easymusic.h"

static MCIERROR last_error;

//----------------通用----------------
//获取最近一次调用时的 MCI 错误码。
MCIERROR em_get_last_error()
{
	return last_error;
}

//获取指定 MCI 错误码对应的错误描述。
//参数：
//	error_code -> 错误码
//返回值：
//	获取到的错误描述的字符串
char* em_get_error(MCIERROR error_code) 
{
	char* str = malloc(sizeof(char) * 1024);
	bool result = (bool)mciGetErrorStringA(error_code, str, 1024);
	return result ? str : NULL;
}

//按“分分:秒秒”的格式输出时间
//参数：
//	sec - 输入的秒数
//	output - 输出的结果，传入字符型数组指针即可。需保证数组长度大于格式化输出后字符串的长度，否则可能会造成未知影响。
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
	//首先估计歌词的行数，以分配空间
	//通过换行的数量来估计歌词行数
	const char *ptr = str; //输入字符串的指针
	int count = 0; // \n 即换行符的数量
	while (ptr != NULL && ptr <= str && ptr - str < (int)strlen(str))
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
		lyric->lyric_array[lyric_line_index] = malloc(strlen(text_line) + 1);
		if (lyric->lyric_array[lyric_line_index] == NULL)
			return EM_ERR_ALLOC_FAILED;
		strcpy(lyric->lyric_array[lyric_line_index], text_line);
		lyric_line_index++;
	next:
		line = strtok(NULL, "\n"); //按行分割字符串
		
	}
	free(str_new);

	return EM_ERR_OK;
}

Lyric *lyric_create_from_string(const char *str)
{
	Lyric *lyric = malloc(sizeof(Lyric));
	memset(lyric, 0, sizeof(Lyric));
	if (lyric == NULL)
		return NULL;
	lyric_init(lyric, str);
	return lyric;
}

EM_ERR lyric_update(Lyric *lyric, double position, char **current_line)
{
	//思路：遍历所有歌词，找到离当前时间点最近且不超过当前时间的歌词行
	double min_delta_time = 999999.0;
	for (size_t i = 0; i < lyric->length; i++)
	{
		double result = position - lyric->time_array[i];
		if (result > 0 && result < min_delta_time)
		{
			min_delta_time = result;
			*current_line = lyric->lyric_array[i]; //把时间差最小的歌词行字符串指针保存下来
		}
	}
	
	return min_delta_time != 999999.0; //如果 min_delta_time 还是 999999.0 说没找到
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

EM_ERR player_stop(MusicPlayer *player)
{

}

//相对于当前位置前进/后退。
//参数：
//	player - MusicPlayer 结构体指针
//	relative_sec - 相对移动的秒数。正数表示前进，负数表示后退。
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
//参数：
//	player - MusicPlayer 结构体指针
//	position_sec - 要移动到的位置，单位为秒。
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

	// Query position
	params.dwItem = MCI_STATUS_POSITION;
	last_error = mciSendCommandA(player->device_id, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID)&params);
	*seconds = params.dwReturn / 1000.0;
	return EM_ERR_OK;
}

//获取当前打开的媒体文件的总时长。简化版
inline double player_length_get_(MusicPlayer *player)
{
	double ret = 0;
	player_length_get(player, &ret);
	return ret;
}

//获取当前播放的位置。简化版
inline double player_position_get_(MusicPlayer *player)
{
	double ret = 0;
	player_position_get(player, &ret);
	return ret;
}

//
inline bool player_status_playing_(MusicPlayer *player)
{
	PLAYER_STATUS status = PLAYER_STATUS_UNKNOWN;
	player_status(player, &status);
	return status == PLAYER_STATUS_PLAYING;
}


#include <stdio.h>
#define EXAMPLE_MP3_FILE "D:\\三无MarBlue - 权御天下（Cover 洛天依）.mp3"
#define EXAMPLE_NETWORK_FILE "http://music.163.com/song/media/outer/url?id=1826139261.mp3"
//#include "easymusic.h"

//最简单的例子
void hello_world()
{
	EM_ERR err = 0;
	MusicPlayer *p = player_create(); //创建播放器
	err = player_open(p, EXAMPLE_MP3_FILE); //打开文件，播放音乐之前需要先打开
	player_play(p); //播放音乐

	Sleep(10000); //延时 10000 毫秒（= 10 秒），10 秒后会自动退出
	player_close(p); //不用了之前要关闭文件
}

//开始播放，一直到播放完毕才退出
void play_until_end()
{
	EM_ERR err = 0;
	MusicPlayer *p = player_create(); //创建播放器
	err = player_open(p, EXAMPLE_MP3_FILE); //打开文件，播放音乐之前需要先打开
	player_play(p); //播放音乐
	//上面是一样的初始化部分

	//player_status_playing_(p) 的作用是查询播放器是否正在播放，如果没在播放（播放完成会自动变为停止状态），就会返回假，循环退出
	while (player_status_playing_(p))
	{
		Sleep(100); //加这一句是为了避免 CPU 占用过高
	}
}

//加上简单的控制系统
void simple_control()
{
	EM_ERR err = 0;
	MusicPlayer *p = player_create(); //创建播放器
	err = player_open(p, EXAMPLE_MP3_FILE); //打开文件，播放音乐之前需要先打开
	player_play(p); //播放音乐
	//上面是一样的初始化部分

	char opt = '\0';
	while (opt != 'q')
	{
		system("cls"); //清屏
		puts("p - 暂停/播放");
		puts("a - 前进 5s");
		puts("d - 后退 5s");
		puts("w - 音量 +20%");
		puts("s - 音量 -20%");
		puts("q - 退出");
		printf("请输入：");
		opt = getchar();

		PLAYER_STATUS status;
		switch (opt)
		{
		case 'p':
			player_status(p, &status);
			if (status == PLAYER_STATUS_PAUSED)
				player_play(p);
			else
				player_pause(p);
			break;
		case 'a':
			player_seek_relative(p, 5.0); //TODO 修好这个
			EM_CHECK_AUTO();
			break;
		case 'd':
			player_seek_relative(p, -5.0);
			EM_CHECK_AUTO();
			break;
		case 'w':
			break;
		case 's':
			break;
		case 'q':
			break;
		case 'r':
			break;
		default:
			break;
		}
	}
}

//更加高级一点的 UI
void advanced_control()
{
	EM_ERR err = 0;
	MusicPlayer *p = player_create(); //创建播放器
	player_open(p, EXAMPLE_MP3_FILE); //打开文件，播放音乐之前需要先打开
	player_play(p); //播放音乐
	//上面是一样的初始化部分
	Sleep(10);
	printf("播放中...\n");
	while (player_status_playing_(p))
	{
		EM_CHECK_AUTO();

		//---当前时间部分开始---
		char position_str[6];
		em_format_time(player_position_get_(p), (char*)&position_str, 6);
		printf("%s  ", position_str);
		//---当前时间部分结束---
		//---进度条部分开始--- 
		int progress_length = 50; //总长度
		int progress_finished_length = (int)(1.0*player_position_get_(p) / player_length_get_(p) * progress_length); //已完成长度
		int progress_unfinished_length = progress_length - progress_finished_length; //未完成长度
		putchar('[');
		for (int i = 0; i < progress_finished_length; i++)
			putchar('=');
		for (int i = 0; i < progress_unfinished_length; i++)
			putchar(' ');
		putchar(']');
		//---进度条部分结束---
		//---总长度部分开始---
		char length_str[6];
		em_format_time(player_length_get_(p), (char*)&length_str);
		printf("  %s", length_str);
		//---总长度部分结束---

		putchar('\r');
	}
	system("cls");
	printf("已停止。");
	getchar();
}

//播放在线音乐
void hello_network()
{
	EM_ERR err = 0;
	MusicPlayer *p = player_create(); //创建播放器
	err = player_open(p, EXAMPLE_NETWORK_FILE); //打开文件，播放音乐之前需要先打开
	EM_CHECK_AUTO();
	player_play(p); //播放音乐
	EM_CHECK_AUTO();
	while (player_status_playing_(p))
	{
		Sleep(100); //加这一句是为了避免 CPU 占用过高
	}
	player_close(p); //不用了之前要关闭文件
}

int main2()
{
	const char *lrc = "[00:00.000]LRC-musicenc.com\n[00:00]三无Marblue - 权御天下\n[00:00]作词：ST\n[00:00]作曲：乌龟\n[00:22]东汉末狼烟不休\n[00:24]常侍乱朝野陷\n[00:25]阿瞒挟天子令诸侯\n[00:27]踞江东志在九州\n[00:29]继祖业承父兄\n[00:30]既冕主吴越万兜鍪\n[00:33]纵天下几变春秋\n[00:34]稳东南面中原\n[00:35]水师锁长江抗曹刘\n[00:38]镇赤壁雄风赳赳\n[00:40]夺荆楚抚山越\n[00:41]驱金戈铁马灭仇雠\n[00:53]紫发髯碧色眼眸\n[00:55]射猛虎倚黄龙\n[00:56]胆识过凡人谁敌手\n[00:58]御天下半百之久\n[01:00]选贤臣任能将\n[01:01]覆江东云雨尽风流\n[01:04]千秋过再难回首\n[01:05]问古今兴亡事\n[01:07]几人耀青史芳名留\n[01:09]笑谈间云烟已旧\n[01:10]终留下万古叹\n[01:12]生子该当如孙仲谋\n[01:14]运帷幄英雄几拂袖\n[01:16]阴谋阳谋明仇暗斗\n[01:18]化作一江浊浪东流\n[01:21]君不见军赤壁\n[01:22]纵野火铁索连环\n[01:24]也不见御北敌\n[01:25]联西蜀长江上鏖战\n[01:26]继遗志领江东\n[01:27]屹立于神州东南\n[01:29]尽心力洒英血\n[01:30]展伟业剑气指苍天\n[01:31]军帐内公瑾智\n[01:33]张昭谋奇策频献\n[01:34]沙场上太史勇\n[01:35]甘宁霸一骑当十千\n[01:36]纵使有千万种\n[01:38]寂寞和孤单相伴\n[01:39]既受终冠帝冕\n[01:40]龙椅上成败也笑看\n[01:53]铁瓮城难攻易守\n[01:55]旌旗立苍空蔽\n[01:56]逾百千雄师万蒙舟\n[01:58]善制衡眼光独秀\n[02:00]擢鲁肃劝阿蒙\n[02:01]聚贤成霸业名利收\n[02:03]固疆土施德恩厚\n[02:05]军心定百姓安\n[02:06]富国又强兵重耕耨\n[02:08]交远好未雨绸缪\n[02:10]联南洋合林邑\n[02:11]行军远渡海驻夷洲\n[02:14]残垣下枯木雕朽\n[02:15]想当年麦城边\n[02:17]截兵缚关羽终其寿\n[02:19]凭栏倚横看吴钩\n[02:21]叹乱世几时了\n[02:22]天下归一统没其咎\n[02:24]称帝王壮心仍稠\n[02:26]却无奈自孤傲\n[02:27]同室亦操戈子嗣斗\n[02:29]千年后恚恨徒留\n[02:31]再何寻军帐里\n[02:32]将士聚欢饮赏箜篌\n[02:34]运帷幄英雄几拂袖\n[02:37]阴谋阳谋明仇暗斗\n[02:39]化作一江浊浪东流\n[02:41]君不见吕子明\n[02:43]踏轻舟白衣渡川\n[02:44]也不见陆伯言\n[02:45]烧联营火光上冲天\n[02:47]善制衡选贤臣\n[02:48]任能将共谋江山\n[02:49]听忠言摒逆语\n[02:50]树威严宝剑斫书案\n[02:52]夺荆州抗刘备合\n[02:53]曹操共克襄樊\n[02:54]守夷陵任陆逊\n[02:55]剿敌军\n[02:56]火计破蜀胆\n[02:57]固江河成帝业立\n[02:58]国家终归于乱\n[03:00]光阴逝千载过功\n[03:01]成者都付笑谈间\n[03:13]君不见军赤壁纵\n[03:14]野火铁索连环\n[03:15]也不见御北敌联\n[03:17]西蜀长江上鏖战\n[03:18]继遗志领江东\n[03:19]屹立于神州东南\n[03:20]尽心力洒英血展\n[03:22]伟业剑指苍天\n[03:23]君不见吕子明\n[03:24]踏轻舟白衣渡川\n[03:25]也不见陆伯言\n[03:27]烧联营火光上冲天\n[03:28]善制衡选贤臣任\n[03:29]能将共谋江山\n[03:31]听忠言摒逆语\n[03:32]树威严宝剑斫书案\n[03:33]纵使有千万种\n[03:34]寂寞和孤单相伴\n[03:36]既受终冠帝冕\n[03:37]龙椅上成败也笑看\n[03:39]固江河成帝业立\n[03:40]国家终归于乱\n[03:41]光阴逝千载\n[03:42]过功成者\n[03:43]都付笑谈间";
	Lyric *lyric = lyric_create_from_string(lrc);

	EM_ERR err = 0;
	MusicPlayer *p = player_create();
	err = player_open(p, EXAMPLE_MP3_FILE);
	player_play(p);
					//上面是一样的初始化部分
	char *current_line = NULL;
	while (player_status_playing_(p))
	{
		putchar('\r');
		lyric_update(lyric, player_position_get_(p), &current_line);
		printf("%s", current_line);
		for (size_t i = 0; i < 50; i++)
			putchar('\0');
	}
	system("pause");
	return 0;
}
