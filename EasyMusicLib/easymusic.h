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
	double *time_array; //double 数组
	char **lyric_array; //字符串数组
} Lyric;

#define EM_CHECK_AUTO(expression) EM_CHECK_MCI(em_get_last_error(), expression)

#define EM_CHECK_MCI(err_code, expression) \
    if (err_code != 0) { \
        char* errStr = em_get_error(err_code); \
        printf("MCI 错误: %s（错误码：%d）\n", errStr, err_code); \
        free(errStr); \
        expression; \
    }

//-----常量定义-----
typedef int EM_ERR; //错误代码
#define EM_ERR_OK 0 //无错误
#define EM_ERR_UNSPPORTED_FORMAT -1 //文件格式不支持
#define EM_ERR_FAILED_TO_LOAD -2 //载入文件失败
#define EM_ERR_ALLOC_FAILED -3
#define EM_ERR_PARAM_INVAILD -4 //参数无效
#define EM_ERR_RAW -100
#define EM_ERR_UNKNOWN -110


typedef enum
{
	PLAYER_STATUS_UNKNOWN = -1,
	PLAYER_STATUS_STOPPED,
	PLAYER_STATUS_PLAYING,
	PLAYER_STATUS_PAUSED,
	PLAYER_STATUS_BUFFERING,
} PLAYER_STATUS; //状态码


//从字符串中创建 Lyric 结构体
//	参数：
//		str - .lrc 歌词内容字符串
//	返回值：
//		Lyric 变量
Lyric *lyric_create_from_string(const char *str);
//更新歌词，传入当前时间，返回当前应该显示的歌词
//	参数：
//		lyric - Lyric 结构体
//		position - 当前位置（秒）
//		current_line - 返回值，当前显示的歌词。传入一个 char 类型的二级指针。
//	返回值：
//		调用是否成功。
EM_ERR lyric_update(Lyric *lyric, double position, char **current_line);



//创建一个播放器
//	返回值：创建的播放器实例。如果失败，则返回 NULL。
MusicPlayer *player_create();
//打开一个文件。
//	参数：
//		player -> MusicPlayer 结构体的指针
//		file_path -> 文件路径
//	返回值：
//		错误码，即 EM_ERR_ 开头的常量。
EM_ERR player_open(MusicPlayer *player, const char *file_path);
//播放。
//	参数：
//		player -> EasyPlayer 结构体的指针
//	返回值：
//		错误码，即 EM_ERR_ 开头的常量。
EM_ERR player_play(MusicPlayer *player);
//获取播放器的总长度（秒）。
//	参数：
//		player - MusicPlayer 结构体的指针
//		seconds - 返回值，播放器的总长度（秒）。传入一个 double 类型的指针。
//	返回值：
//		错误码，即 EM_ERR_ 开头的常量。
EM_ERR player_length_get(MusicPlayer *player, double *seconds);

//获取当前播放位置（秒）。
//	参数：
// player - MusicPlayer 结构体的指针
//		seconds - 返回值，当前播放位置（秒）。传入一个 double 类型的指针。
//	返回值：
//		错误码，即 EM_ERR_ 开头的常量。
EM_ERR player_position_get(MusicPlayer *player, double *seconds);

//获取播放器的总长度（秒）。
//	参数：
//		player - MusicPlayer 结构体的指针
//	返回值：
//		播放器的总长度（秒）。
 double player_length_get_(MusicPlayer *player);

//获取当前播放位置（秒）。
//	参数：
//		player - MusicPlayer 结构体的指针
//	返回值：
//		当前播放位置（秒）。
inline double player_position_get_(MusicPlayer *player);

//获取播放器的播放状态。
//	参数：
//		player - MusicPlayer 结构体的指针
//	返回值：
//		当前播放器是否正在播放。
inline bool player_status_playing_(MusicPlayer *player);