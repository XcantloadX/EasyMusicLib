#include "easymusic.h"

static MCIERROR last_error;

//----------------ͨ��----------------
//��ȡ���һ�ε���ʱ�� MCI �����롣
MCIERROR em_get_last_error()
{
	return last_error;
}

//��ȡָ�� MCI �������Ӧ�Ĵ���������
//������
//	error_code -> ������
//����ֵ��
//	��ȡ���Ĵ����������ַ���
char* em_get_error(MCIERROR error_code) 
{
	char* str = malloc(sizeof(char) * 1024);
	bool result = (bool)mciGetErrorStringA(error_code, str, 1024);
	return result ? str : NULL;
}

//�����ַ�:���롱�ĸ�ʽ���ʱ��
//������
//	sec - ���������
//	output - ����Ľ���������ַ�������ָ�뼴�ɡ��豣֤���鳤�ȴ��ڸ�ʽ��������ַ����ĳ��ȣ�������ܻ����δ֪Ӱ�졣
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
	//���ȹ��Ƹ�ʵ��������Է���ռ�
	//ͨ�����е����������Ƹ������
	const char *ptr = str; //�����ַ�����ָ��
	int count = 0; // \n �����з�������
	while (ptr != NULL && ptr <= str && ptr - str < (int)strlen(str))
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
		lyric->lyric_array[lyric_line_index] = malloc(strlen(text_line) + 1);
		if (lyric->lyric_array[lyric_line_index] == NULL)
			return EM_ERR_ALLOC_FAILED;
		strcpy(lyric->lyric_array[lyric_line_index], text_line);
		lyric_line_index++;
	next:
		line = strtok(NULL, "\n"); //���зָ��ַ���
		
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
	//˼·���������и�ʣ��ҵ��뵱ǰʱ�������Ҳ�������ǰʱ��ĸ����
	double min_delta_time = 999999.0;
	for (size_t i = 0; i < lyric->length; i++)
	{
		double result = position - lyric->time_array[i];
		if (result > 0 && result < min_delta_time)
		{
			min_delta_time = result;
			*current_line = lyric->lyric_array[i]; //��ʱ�����С�ĸ�����ַ���ָ�뱣������
		}
	}
	
	return min_delta_time != 999999.0; //��� min_delta_time ���� 999999.0 ˵û�ҵ�
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

EM_ERR player_stop(MusicPlayer *player)
{

}

//����ڵ�ǰλ��ǰ��/���ˡ�
//������
//	player - MusicPlayer �ṹ��ָ��
//	relative_sec - ����ƶ���������������ʾǰ����������ʾ���ˡ�
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
//������
//	player - MusicPlayer �ṹ��ָ��
//	position_sec - Ҫ�ƶ�����λ�ã���λΪ�롣
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

	// Query position
	params.dwItem = MCI_STATUS_POSITION;
	last_error = mciSendCommandA(player->device_id, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID)&params);
	*seconds = params.dwReturn / 1000.0;
	return EM_ERR_OK;
}

//��ȡ��ǰ�򿪵�ý���ļ�����ʱ�����򻯰�
inline double player_length_get_(MusicPlayer *player)
{
	double ret = 0;
	player_length_get(player, &ret);
	return ret;
}

//��ȡ��ǰ���ŵ�λ�á��򻯰�
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
#define EXAMPLE_MP3_FILE "D:\\����MarBlue - Ȩ�����£�Cover ��������.mp3"
#define EXAMPLE_NETWORK_FILE "http://music.163.com/song/media/outer/url?id=1826139261.mp3"
//#include "easymusic.h"

//��򵥵�����
void hello_world()
{
	EM_ERR err = 0;
	MusicPlayer *p = player_create(); //����������
	err = player_open(p, EXAMPLE_MP3_FILE); //���ļ�����������֮ǰ��Ҫ�ȴ�
	player_play(p); //��������

	Sleep(10000); //��ʱ 10000 ���루= 10 �룩��10 �����Զ��˳�
	player_close(p); //������֮ǰҪ�ر��ļ�
}

//��ʼ���ţ�һֱ��������ϲ��˳�
void play_until_end()
{
	EM_ERR err = 0;
	MusicPlayer *p = player_create(); //����������
	err = player_open(p, EXAMPLE_MP3_FILE); //���ļ�����������֮ǰ��Ҫ�ȴ�
	player_play(p); //��������
	//������һ���ĳ�ʼ������

	//player_status_playing_(p) �������ǲ�ѯ�������Ƿ����ڲ��ţ����û�ڲ��ţ�������ɻ��Զ���Ϊֹͣ״̬�����ͻ᷵�ؼ٣�ѭ���˳�
	while (player_status_playing_(p))
	{
		Sleep(100); //����һ����Ϊ�˱��� CPU ռ�ù���
	}
}

//���ϼ򵥵Ŀ���ϵͳ
void simple_control()
{
	EM_ERR err = 0;
	MusicPlayer *p = player_create(); //����������
	err = player_open(p, EXAMPLE_MP3_FILE); //���ļ�����������֮ǰ��Ҫ�ȴ�
	player_play(p); //��������
	//������һ���ĳ�ʼ������

	char opt = '\0';
	while (opt != 'q')
	{
		system("cls"); //����
		puts("p - ��ͣ/����");
		puts("a - ǰ�� 5s");
		puts("d - ���� 5s");
		puts("w - ���� +20%");
		puts("s - ���� -20%");
		puts("q - �˳�");
		printf("�����룺");
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
			player_seek_relative(p, 5.0); //TODO �޺����
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

//���Ӹ߼�һ��� UI
void advanced_control()
{
	EM_ERR err = 0;
	MusicPlayer *p = player_create(); //����������
	player_open(p, EXAMPLE_MP3_FILE); //���ļ�����������֮ǰ��Ҫ�ȴ�
	player_play(p); //��������
	//������һ���ĳ�ʼ������
	Sleep(10);
	printf("������...\n");
	while (player_status_playing_(p))
	{
		EM_CHECK_AUTO();

		//---��ǰʱ�䲿�ֿ�ʼ---
		char position_str[6];
		em_format_time(player_position_get_(p), (char*)&position_str, 6);
		printf("%s  ", position_str);
		//---��ǰʱ�䲿�ֽ���---
		//---���������ֿ�ʼ--- 
		int progress_length = 50; //�ܳ���
		int progress_finished_length = (int)(1.0*player_position_get_(p) / player_length_get_(p) * progress_length); //����ɳ���
		int progress_unfinished_length = progress_length - progress_finished_length; //δ��ɳ���
		putchar('[');
		for (int i = 0; i < progress_finished_length; i++)
			putchar('=');
		for (int i = 0; i < progress_unfinished_length; i++)
			putchar(' ');
		putchar(']');
		//---���������ֽ���---
		//---�ܳ��Ȳ��ֿ�ʼ---
		char length_str[6];
		em_format_time(player_length_get_(p), (char*)&length_str);
		printf("  %s", length_str);
		//---�ܳ��Ȳ��ֽ���---

		putchar('\r');
	}
	system("cls");
	printf("��ֹͣ��");
	getchar();
}

//������������
void hello_network()
{
	EM_ERR err = 0;
	MusicPlayer *p = player_create(); //����������
	err = player_open(p, EXAMPLE_NETWORK_FILE); //���ļ�����������֮ǰ��Ҫ�ȴ�
	EM_CHECK_AUTO();
	player_play(p); //��������
	EM_CHECK_AUTO();
	while (player_status_playing_(p))
	{
		Sleep(100); //����һ����Ϊ�˱��� CPU ռ�ù���
	}
	player_close(p); //������֮ǰҪ�ر��ļ�
}

int main2()
{
	const char *lrc = "[00:00.000]LRC-musicenc.com\n[00:00]����Marblue - Ȩ������\n[00:00]���ʣ�ST\n[00:00]�������ڹ�\n[00:22]����ĩ���̲���\n[00:24]�����ҳ�Ұ��\n[00:25]����Ю���������\n[00:27]�Ὥ��־�ھ���\n[00:29]����ҵ�и���\n[00:30]��������Խ����\n[00:33]�����¼��䴺��\n[00:34]�ȶ�������ԭ\n[00:35]ˮʦ������������\n[00:38]�����۷�����\n[00:40]�ᾣ����ɽԽ\n[00:41]��������������\n[00:53]�Ϸ��ױ�ɫ����\n[00:55]���ͻ��л���\n[00:56]��ʶ������˭����\n[00:58]�����°��֮��\n[01:00]ѡ�ͳ����ܽ�\n[01:01]���������꾡����\n[01:04]ǧ������ѻ���\n[01:05]�ʹŽ�������\n[01:07]����ҫ��ʷ������\n[01:09]Ц̸�������Ѿ�\n[01:10]���������̾\n[01:12]���Ӹõ�������ı\n[01:14]����Ӣ�ۼ�����\n[01:16]��ı��ı���𰵶�\n[01:18]����һ�����˶���\n[01:21]�����������\n[01:22]��Ұ����������\n[01:24]Ҳ����������\n[01:25]�����񳤽�����ս\n[01:26]����־�콭��\n[01:27]���������ݶ���\n[01:29]��������ӢѪ\n[01:30]չΰҵ����ָ����\n[01:31]�����ڹ����\n[01:33]����ı���Ƶ��\n[01:34]ɳ����̫ʷ��\n[01:35]������һ�ﵱʮǧ\n[01:36]��ʹ��ǧ����\n[01:38]��į�͹µ����\n[01:39]�����չڵ���\n[01:40]�����ϳɰ�ҲЦ��\n[01:53]���ͳ��ѹ�����\n[01:55]������Կձ�\n[01:56]���ǧ��ʦ������\n[01:58]���ƺ��۹����\n[02:00]ߪ³��Ȱ����\n[02:01]���ͳɰ�ҵ������\n[02:03]�̽���ʩ�¶���\n[02:05]���Ķ����հ�\n[02:06]������ǿ���ظ���\n[02:08]��Զ��δ�����\n[02:10]�����������\n[02:11]�о�Զ�ɺ�פ����\n[02:14]��ԫ�¿�ľ����\n[02:15]�뵱����Ǳ�\n[02:17]�ر�������������\n[02:19]ƾ���кῴ�⹳\n[02:21]̾������ʱ��\n[02:22]���¹�һͳû���\n[02:24]�Ƶ���׳���Գ�\n[02:26]ȴ�����Թ°�\n[02:27]ͬ����ٸ����ö�\n[02:29]ǧ�����ͽ��\n[02:31]�ٺ�Ѱ������\n[02:32]��ʿ�ۻ���������\n[02:34]����Ӣ�ۼ�����\n[02:37]��ı��ı���𰵶�\n[02:39]����һ�����˶���\n[02:41]������������\n[02:43]̤���۰��¶ɴ�\n[02:44]Ҳ����½����\n[02:45]����Ӫ����ϳ���\n[02:47]���ƺ�ѡ�ͳ�\n[02:48]���ܽ���ı��ɽ\n[02:49]������������\n[02:50]�����ϱ�����鰸\n[02:52]�ᾣ�ݿ�������\n[02:53]�ܲٹ����差\n[02:54]��������½ѷ\n[02:55]�˵о�\n[02:56]�������\n[02:57]�̽��ӳɵ�ҵ��\n[02:58]�����չ�����\n[03:00]������ǧ�ع���\n[03:01]���߶���Ц̸��\n[03:13]�������������\n[03:14]Ұ����������\n[03:15]Ҳ������������\n[03:17]���񳤽�����ս\n[03:18]����־�콭��\n[03:19]���������ݶ���\n[03:20]��������ӢѪչ\n[03:22]ΰҵ��ָ����\n[03:23]������������\n[03:24]̤���۰��¶ɴ�\n[03:25]Ҳ����½����\n[03:27]����Ӫ����ϳ���\n[03:28]���ƺ�ѡ�ͳ���\n[03:29]�ܽ���ı��ɽ\n[03:31]������������\n[03:32]�����ϱ�����鰸\n[03:33]��ʹ��ǧ����\n[03:34]��į�͹µ����\n[03:36]�����չڵ���\n[03:37]�����ϳɰ�ҲЦ��\n[03:39]�̽��ӳɵ�ҵ��\n[03:40]�����չ�����\n[03:41]������ǧ��\n[03:42]��������\n[03:43]����Ц̸��";
	Lyric *lyric = lyric_create_from_string(lrc);

	EM_ERR err = 0;
	MusicPlayer *p = player_create();
	err = player_open(p, EXAMPLE_MP3_FILE);
	player_play(p);
					//������һ���ĳ�ʼ������
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
