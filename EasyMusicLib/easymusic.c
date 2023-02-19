#include "easymusic.h"

static MCIERROR last_error;

//----------------ͨ��----------------
//��ȡ���һ�ε���ʱ�� MCI �����롣
MCIERROR em_get_last_error()
{
	return last_error;
}

//��ȡָ�� MCI �������Ӧ�Ĵ���������
char* em_get_error(MCIERROR error_code) 
{
	char* str = malloc(sizeof(char) * 1024);
	bool result = (bool)mciGetErrorStringA(error_code, str, 1024);
	return result ? str : NULL;
}

//�����ַ�:���롱�ĸ�ʽ���ʱ��
void em_format_time(double sec, char *output)
{
	int mm = (int)(sec / 60);
	int ss = (int)sec % 60;
	sprintf(output, "%02d:%02d", mm, ss);
}

//----------------���----------------

//Ϊ Lyric �ṹ�����ռ䲢���� .lrc ����ı�
static EM_ERR lyric_init(Lyric *lyric, const char *str)
{
	if (str == NULL) //���������©
		return EM_ERR_PARAM_INVAILD;
	//���ȹ��Ƹ�ʵ��������Է���ռ�
	//ͨ�����е����������Ƹ������
	const char *ptr = str; //�����ַ�����ָ��
	int count = 0; // \n �����з�������
	while (ptr != NULL && ptr >= str && ptr - str < (int)strlen(str))
	{
		ptr = strchr(ptr, '\n');
		count++;
		ptr++;
	}
	
	//����ռ�
	lyric->length = count;
	lyric->time_array = calloc(sizeof(double), count);
	lyric->lyric_array = calloc(sizeof(char*), count);

	//.lrc ���ÿһ�еĸ�ʽ�����£�[03:01.99]xxxx
	//��ͷ��ʱ����ܳ��ֶ�Σ�[00:01.80][00:04.92][01:18.27][01:20.97][03:15.35][03:18.30]xxxx
	//TODO ֧�ֶ��ʱ��

	//���� strtok ��ı�ԭ�ַ�����������Ҫ�ȸ��Ʊ���һ��
	char *str_new = malloc(strlen(str) + 1);
	if (str_new == NULL)
		return EM_ERR_ALLOC_FAILED;
	strcpy(str_new, str);

	//��ʼ��ÿһ�н��з���
	char *line = strtok(str_new, "\n"); //�� \n �ָ��ַ���
	int lyric_line_index = 0;
	while (line != NULL)
	{
		int index = 0;
		if (line[index] != '[') //������ [ ��ͷ�ĺ��Ե�
			goto next;
		while (line[index] != ']')
		{
			index++;
			if (index > strlen(line))
				goto next;
		}
		
		//��ȡ��ʱ�䲢ת��Ϊ��
		int mm = 0;
		double ss = 0;
		sscanf(line, "[%d:%lf]", &mm, &ss);
		lyric->time_array[lyric_line_index] = mm * 60 + ss; //�������ʱ��

		//��ȡ�����
		char *text_line = line + index + 1;
		lyric->lyric_array[lyric_line_index] = malloc(strlen(text_line) + 2);
		if (lyric->lyric_array[lyric_line_index] == NULL)
			return EM_ERR_ALLOC_FAILED;
		strcpy(lyric->lyric_array[lyric_line_index], text_line);
		lyric_line_index++;
		if (lyric_line_index > lyric->length)
			system("pause");
	next:
		line = strtok(NULL, "\n"); //���зָ��ַ���
		
	}
	free(str_new);

	return EM_ERR_OK;
}

//���ַ����д��� Lyric �ṹ��
Lyric *lyric_create_from_string(const char *str)
{
	Lyric *lyric = malloc(sizeof(Lyric));
	memset(lyric, 0, sizeof(Lyric));
	if (lyric == NULL)
		return NULL;
	lyric_init(lyric, str);
	return lyric;
}

//��ȡָ���еĸ��
EM_ERR lyric_get(Lyric *lyric, int index, char **current_line)
{
	if (index < 0 || index > lyric->length)
		return EM_ERR_PARAM_INVAILD;

	*current_line = lyric->lyric_array[index];
	return EM_ERR_OK;
}

//���¸�ʣ���ȡ��ǰ��ʵ��±�
EM_ERR lyric_update_index(Lyric *lyric, double position, int *current_line_index)
{
	//˼·���������и�ʣ��ҵ��뵱ǰʱ�������Ҳ�������ǰʱ��ĸ����
	double min_delta_time = 999999.0;
	for (size_t i = 0; i < lyric->length; i++)
	{
		double result = position - lyric->time_array[i];
		if (result > 0 && result < min_delta_time)
		{
			min_delta_time = result;
			*current_line_index =i; //��ʱ�����С�ĸ���е��±걣������
		}
	}

	return min_delta_time != 999999.0; //��� min_delta_time ���� 999999.0 ˵û�ҵ�
}

//���¸�ʣ���ȡ��ǰ����ı�
EM_ERR lyric_update(Lyric *lyric, double position, char **current_line)
{
	int index = 0;
	EM_ERR err = 0;
	err = lyric_update_index(lyric, position, &index);
	if (err != EM_ERR_OK)
		return err;

	return lyric_get(lyric, index, current_line);
}

//----------------������----------------

MusicPlayer *player_create()
{
	MusicPlayer *player = malloc(sizeof(MusicPlayer));
	if (player == NULL)
		return NULL;
	memset(player, 0, sizeof(MusicPlayer));
	return player;
}

//��һ���ļ���
//	������
//		player -> EasyPlayer �ṹ���ָ��
//		file_path -> �ļ�·��
//	����ֵ��
//		�����룬�� EM_ERR_ ��ͷ�ĳ�����
EM_ERR player_open(MusicPlayer *player, const char *file_path)
{
	MCI_OPEN_PARMS params;
	params.lpstrDeviceType = NULL; //�豸����Ϊ�գ���ʾ�Զ��ƶ�
	params.lpstrElementName = (LPCWSTR)file_path;
	last_error = mciSendCommandA((MCIDEVICEID)NULL, MCI_OPEN, MCI_OPEN_ELEMENT, (DWORD)(LPVOID)&params);
	if (last_error != 0)
		return EM_ERR_FAILED_TO_LOAD;
	player->device_id = params.wDeviceID;
	return EM_ERR_OK;
}

//�رղ�����
EM_ERR player_close(MusicPlayer *player)
{
	last_error = mciSendCommand(player->device_id, MCI_CLOSE, 0, NULL);
	return EM_ERR_OK;
}

//���š�
EM_ERR player_play(MusicPlayer *player)
{
	MCI_PLAY_PARMS params = { 0 };
	last_error = mciSendCommandA(player->device_id, MCI_PLAY, (DWORD_PTR)NULL, (DWORD_PTR)&params);
	if (last_error == 0)
		return EM_ERR_OK;
	else
		return EM_ERR_UNKNOWN;
}

//��ָ����ʼλ�ò��ŵ�ָ������λ��
EM_ERR player_play_advanced(MusicPlayer *player, double fromSec, double toSec)
{

}

//��ͣ
EM_ERR player_pause(MusicPlayer *player)
{
	last_error = mciSendCommandA(player->device_id, MCI_PAUSE, 0, (DWORD_PTR)NULL);
	return EM_ERR_OK;
}

//ֹͣ
EM_ERR player_stop(MusicPlayer *player)
{
	last_error = mciSendCommandA(player->device_id, MCI_STOP, 0, (DWORD_PTR)NULL);
	return EM_ERR_OK;
}

//����ڵ�ǰλ��ǰ��/���ˡ�
EM_ERR player_seek_relative(MusicPlayer *player, double relative_sec)
{
	double pos;
	EM_ERR err;
	err = player_position_get(player, &pos);
	if (err != EM_ERR_OK)
		return err;
	player_seek(player, pos + relative_sec);
}

//���ò���λ�õ�ָ��ʱ�䡣
EM_ERR player_seek(MusicPlayer *player, double position_sec) 
{
	MCI_SEEK_PARMS mciSeekParms;
	mciSeekParms.dwTo = (DWORD)(position_sec * 1000);
	last_error = mciSendCommandA(player->device_id, MCI_SEEK | MCI_TO, (DWORD)NULL, (DWORD_PTR)&mciSeekParms);
}

//��ѯ��ǰ����״̬��
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

//��ȡ��ǰ�򿪵�ý���ļ�����ʱ����
EM_ERR player_length_get(MusicPlayer *player, double *seconds)
{
	MCI_STATUS_PARMS params = { 0 };
	params.dwItem = MCI_STATUS_LENGTH;
	last_error = mciSendCommandA(player->device_id, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID)&params);
	*seconds = params.dwReturn / 1000.0;
	return EM_ERR_OK;
}

//��ȡ��ǰ���ŵ�λ�á�
EM_ERR player_position_get(MusicPlayer *player, double *seconds)
{
	MCI_STATUS_PARMS params = {0};

	params.dwItem = MCI_STATUS_POSITION;
	last_error = mciSendCommandA(player->device_id, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID)&params);
	*seconds = params.dwReturn / 1000.0;
	return EM_ERR_OK;
}

//��ȡ��ǰ�򿪵�ý���ļ�����ʱ�����򻯰�
 double player_length_get_(MusicPlayer *player)
{
	double ret = 0;
	player_length_get(player, &ret);
	return ret;
}

//��ȡ��ǰ���ŵ�λ�á��򻯰�
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