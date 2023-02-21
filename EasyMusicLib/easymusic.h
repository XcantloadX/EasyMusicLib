#pragma comment(lib, "WinMM.Lib")
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <stdbool.h>

//------------�ṹ�嶨��------------
//������
typedef struct _player
{
	MCIDEVICEID device_id;
} MusicPlayer;

//���
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

//------------��������------------
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


//------------��������------------

//��ȡ���һ�ε���ʱ�� MCI �����롣
MCIERROR em_get_last_error();

//��ȡָ�� MCI �������Ӧ�Ĵ���������
//	������
//		error_code -> ������
//	����ֵ��
//		��ȡ���Ĵ����������ַ���
char* em_get_error(MCIERROR error_code);

//�����ַ�:���롱�ĸ�ʽ���ʱ��
//	������
//		sec - ���������
//		output - ����Ľ���������ַ�������ָ�뼴�ɡ��豣֤���鳤�ȴ��ڸ�ʽ��������ַ����ĳ��ȣ�������ܻ����δ֪Ӱ�졣
void em_format_time(double sec, char *output);

//���ַ����д��� Lyric �ṹ��
//	������
//		str - .lrc ��������ַ���
//	����ֵ��
//		Lyric ����
Lyric *lyric_create_from_string(const char *str);
EM_ERR lyric_get(Lyric * lyric, int index, char ** current_line);
EM_ERR lyric_update_index(Lyric * lyric, double position, int * current_line_index);
//���¸�ʣ����뵱ǰʱ�䣬���ص�ǰӦ����ʾ�ĸ��
//	������
//		lyric - Lyric �ṹ��
//		position - ��ǰλ�ã��룩
//		current_line - ����ֵ����ǰ��ʾ�ĸ�ʡ�����һ�� char ���͵Ķ���ָ�뼴�ɡ�
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

//��ͣ��
//	������
//		player -> EasyPlayer �ṹ���ָ��
//	����ֵ��
//		�����룬�� EM_ERR_ ��ͷ�ĳ�����
EM_ERR player_pause(MusicPlayer *player);

//ֹͣ��
//	������
//		player -> EasyPlayer �ṹ���ָ��
//	����ֵ��
//		�����룬�� EM_ERR_ ��ͷ�ĳ�����
EM_ERR player_stop(MusicPlayer *player);

//���ò���λ�õ�ָ��ʱ�䡣
//������
//	player - MusicPlayer �ṹ��ָ��
//	position_sec - Ҫ�ƶ�����λ�ã���λΪ�롣
EM_ERR player_seek(MusicPlayer *player, double position_sec);

//����ڵ�ǰλ��ǰ��/���ˡ�
//������
//	player - MusicPlayer �ṹ��ָ��
//	relative_sec - ����ƶ���������������ʾǰ����������ʾ���ˡ�
EM_ERR player_seek_relative(MusicPlayer *player, double relative_sec);

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
double player_position_get_(MusicPlayer *player);

//��ȡ�������Ĳ���״̬��
//	������
//		player - MusicPlayer �ṹ���ָ��
//	����ֵ��
//		��ǰ�������Ƿ����ڲ��š�
bool player_status_playing_(MusicPlayer *player);