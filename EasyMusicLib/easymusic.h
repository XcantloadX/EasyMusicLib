#pragma comment(lib, "WinMM.Lib")
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>

typedef struct _Player
{
	MCIDEVICEID device_id;
} MusicPlayer;

typedef struct _lyric
{
	int length;
	double *time_array; //double ����
	char **lyric_array; //�ַ�������
} Lyric;

#define EM_CHECK_AUTO(expression) EM_CHECK_MCI(em_get_last_error(), expression)

#define EM_CHECK_MCI(err_code, expression) \
    if (err_code != 0) { \
        char* errStr = em_get_error(err_code); \
        printf("MCI ����: %s�������룺%d��\n", errStr, err_code); \
        free(errStr); \
        expression; \
    }

//-----��������-----
typedef int EM_ERR; //�������
#define EM_ERR_OK 0 //�޴���
#define EM_ERR_UNSPPORTED_FORMAT -1 //�ļ���ʽ��֧��
#define EM_ERR_FAILED_TO_LOAD -2 //�����ļ�ʧ��
#define EM_ERR_ALLOC_FAILED -3
#define EM_ERR_PARAM_INVAILD -4 //������Ч
#define EM_ERR_RAW -100
#define EM_ERR_UNKNOWN -110


typedef enum
{
	PLAYER_STATUS_UNKNOWN = -1,
	PLAYER_STATUS_STOPPED,
	PLAYER_STATUS_PLAYING,
	PLAYER_STATUS_PAUSED,
	PLAYER_STATUS_BUFFERING,
} PLAYER_STATUS; //״̬��


//���ַ����д��� Lyric �ṹ��
//	������
//		str - .lrc ��������ַ���
//	����ֵ��
//		Lyric ����
Lyric *lyric_create_from_string(const char *str);
//���¸�ʣ����뵱ǰʱ�䣬���ص�ǰӦ����ʾ�ĸ��
//	������
//		lyric - Lyric �ṹ��
//		position - ��ǰλ�ã��룩
//		current_line - ����ֵ����ǰ��ʾ�ĸ�ʡ�����һ�� char ���͵Ķ���ָ�롣
//	����ֵ��
//		�����Ƿ�ɹ���
EM_ERR lyric_update(Lyric *lyric, double position, char **current_line);



//����һ��������
//	����ֵ�������Ĳ�����ʵ�������ʧ�ܣ��򷵻� NULL��
MusicPlayer *player_create();
//��һ���ļ���
//	������
//		player -> MusicPlayer �ṹ���ָ��
//		file_path -> �ļ�·��
//	����ֵ��
//		�����룬�� EM_ERR_ ��ͷ�ĳ�����
EM_ERR player_open(MusicPlayer *player, const char *file_path);
//���š�
//	������
//		player -> EasyPlayer �ṹ���ָ��
//	����ֵ��
//		�����룬�� EM_ERR_ ��ͷ�ĳ�����
EM_ERR player_play(MusicPlayer *player);
//��ȡ���������ܳ��ȣ��룩��
//	������
//		player - MusicPlayer �ṹ���ָ��
//		seconds - ����ֵ�����������ܳ��ȣ��룩������һ�� double ���͵�ָ�롣
//	����ֵ��
//		�����룬�� EM_ERR_ ��ͷ�ĳ�����
EM_ERR player_length_get(MusicPlayer *player, double *seconds);

//��ȡ��ǰ����λ�ã��룩��
//	������
// player - MusicPlayer �ṹ���ָ��
//		seconds - ����ֵ����ǰ����λ�ã��룩������һ�� double ���͵�ָ�롣
//	����ֵ��
//		�����룬�� EM_ERR_ ��ͷ�ĳ�����
EM_ERR player_position_get(MusicPlayer *player, double *seconds);

//��ȡ���������ܳ��ȣ��룩��
//	������
//		player - MusicPlayer �ṹ���ָ��
//	����ֵ��
//		���������ܳ��ȣ��룩��
 double player_length_get_(MusicPlayer *player);

//��ȡ��ǰ����λ�ã��룩��
//	������
//		player - MusicPlayer �ṹ���ָ��
//	����ֵ��
//		��ǰ����λ�ã��룩��
inline double player_position_get_(MusicPlayer *player);

//��ȡ�������Ĳ���״̬��
//	������
//		player - MusicPlayer �ṹ���ָ��
//	����ֵ��
//		��ǰ�������Ƿ����ڲ��š�
inline bool player_status_playing_(MusicPlayer *player);