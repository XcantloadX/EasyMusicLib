#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "easymusic.h"

#define EXAMPLE_MP3_FILE "D:\\三无MarBlue - 权御天下（Cover 洛天依）.mp3"
#define EXAMPLE_NETWORK_FILE "http://music.163.com/song/media/outer/url?id=1826139261.mp3"

//最简单的例子
void hello_music()
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
	player_close(p);
}

//更加高级一点的 UI
void advanced_ui()
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

		putchar('\r'); //移动输出光标到开头，这样就能从头开始输出，会直接覆盖掉之前的内容
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

//最简单的歌词显示例子
int hello_lyric()
{
	const char lrc[] = "[00:00]三无Marblue - 权御天下\n[00:00]作词：ST\n[00:00]作曲：乌龟\n[00:22]东汉末狼烟不休\n[00:24]常侍乱朝野陷\n[00:25]阿瞒挟天子令诸侯\n[00:27]踞江东志在九州\n[00:29]继祖业承父兄\n[00:30]既冕主吴越万兜鍪\n[00:33]纵天下几变春秋\n[00:34]稳东南面中原\n[00:35]水师锁长江抗曹刘\n[00:38]镇赤壁雄风赳赳\n[00:40]夺荆楚抚山越\n[00:41]驱金戈铁马灭仇雠\n[00:53]紫发髯碧色眼眸\n[00:55]射猛虎倚黄龙\n[00:56]胆识过凡人谁敌手\n[00:58]御天下半百之久\n[01:00]选贤臣任能将\n[01:01]覆江东云雨尽风流\n[01:04]千秋过再难回首\n[01:05]问古今兴亡事\n[01:07]几人耀青史芳名留\n[01:09]笑谈间云烟已旧\n[01:10]终留下万古叹\n[01:12]生子该当如孙仲谋\n[01:14]运帷幄英雄几拂袖\n[01:16]阴谋阳谋明仇暗斗\n[01:18]化作一江浊浪东流\n[01:21]君不见军赤壁\n[01:22]纵野火铁索连环\n[01:24]也不见御北敌\n[01:25]联西蜀长江上鏖战\n[01:26]继遗志领江东\n[01:27]屹立于神州东南\n[01:29]尽心力洒英血\n[01:30]展伟业剑气指苍天\n[01:31]军帐内公瑾智\n[01:33]张昭谋奇策频献\n[01:34]沙场上太史勇\n[01:35]甘宁霸一骑当十千\n[01:36]纵使有千万种\n[01:38]寂寞和孤单相伴\n[01:39]既受终冠帝冕\n[01:40]龙椅上成败也笑看\n[01:53]铁瓮城难攻易守\n[01:55]旌旗立苍空蔽\n[01:56]逾百千雄师万蒙舟\n[01:58]善制衡眼光独秀\n[02:00]擢鲁肃劝阿蒙\n[02:01]聚贤成霸业名利收\n[02:03]固疆土施德恩厚\n[02:05]军心定百姓安\n[02:06]富国又强兵重耕耨\n[02:08]交远好未雨绸缪\n[02:10]联南洋合林邑\n[02:11]行军远渡海驻夷洲\n[02:14]残垣下枯木雕朽\n[02:15]想当年麦城边\n[02:17]截兵缚关羽终其寿\n[02:19]凭栏倚横看吴钩\n[02:21]叹乱世几时了\n[02:22]天下归一统没其咎\n[02:24]称帝王壮心仍稠\n[02:26]却无奈自孤傲\n[02:27]同室亦操戈子嗣斗\n[02:29]千年后恚恨徒留\n[02:31]再何寻军帐里\n[02:32]将士聚欢饮赏箜篌\n[02:34]运帷幄英雄几拂袖\n[02:37]阴谋阳谋明仇暗斗\n[02:39]化作一江浊浪东流\n[02:41]君不见吕子明\n[02:43]踏轻舟白衣渡川\n[02:44]也不见陆伯言\n[02:45]烧联营火光上冲天\n[02:47]善制衡选贤臣\n[02:48]任能将共谋江山\n[02:49]听忠言摒逆语\n[02:50]树威严宝剑斫书案\n[02:52]夺荆州抗刘备合\n[02:53]曹操共克襄樊\n[02:54]守夷陵任陆逊\n[02:55]剿敌军\n[02:56]火计破蜀胆\n[02:57]固江河成帝业立\n[02:58]国家终归于乱\n[03:00]光阴逝千载过功\n[03:01]成者都付笑谈间\n[03:13]君不见军赤壁纵\n[03:14]野火铁索连环\n[03:15]也不见御北敌联\n[03:17]西蜀长江上鏖战\n[03:18]继遗志领江东\n[03:19]屹立于神州东南\n[03:20]尽心力洒英血展\n[03:22]伟业剑指苍天\n[03:23]君不见吕子明\n[03:24]踏轻舟白衣渡川\n[03:25]也不见陆伯言\n[03:27]烧联营火光上冲天\n[03:28]善制衡选贤臣任\n[03:29]能将共谋江山\n[03:31]听忠言摒逆语\n[03:32]树威严宝剑斫书案\n[03:33]纵使有千万种\n[03:34]寂寞和孤单相伴\n[03:36]既受终冠帝冕\n[03:37]龙椅上成败也笑看\n[03:39]固江河成帝业立\n[03:40]国家终归于乱\n[03:41]光阴逝千载\n[03:42]过功成者\n[03:43]都付笑谈间";
	Lyric *lyric = lyric_create_from_string(lrc); //从我们的字符串里创建歌词

	EM_ERR err = 0;
	MusicPlayer *p = player_create();
	err = player_open(p, EXAMPLE_MP3_FILE);
	player_play(p);
	//上面是一样的初始化部分

	char *current_line = NULL;
	while (player_status_playing_(p))
	{
		putchar('\r'); //移动输出光标到开头，这样就能从头开始输出，会直接覆盖掉之前的内容
		lyric_update(lyric, player_position_get_(p), &current_line); //获取现在应该显示的歌词文本。current_line 是 char*（一级指针），那么 &current_line 就是 char**（二级指针）
		printf("%s", current_line);
		printf("                                 "); //用空格覆盖掉后面的内容。否则，如果这一句比上一句短的话就会出 bug
	}
	system("pause");
	return 0;
}

int main()
{
	while (true)
	{
		int opt = 0;
		system("cls");
		puts("【示例浏览器】");
		puts("1. hello_music：Hello World。最简单的例子。");
		puts("2. play_until_end：完整地放完一首音乐。");
		puts("3. simple_control：暂停、播放、前进、后退");
		puts("4. advanced_ui：”高级界面“");
		puts("5. hello_network：接入互联网！");
		puts("6. hello_lyric：歌词显示");
		printf("请输入选项：");
		scanf("%d", &opt);
		switch (opt)
		{
		case 1:
			system("cls");
			hello_music();
			break;
		case 2:
			system("cls");
			play_until_end();
			break;
		case 3:
			system("cls");
			simple_control();
			break;
		case 4:
			system("cls");
			advanced_ui();
			break;
		case 5:
			system("cls");
			hello_network();
			break;
		case 6:
			system("cls");
			hello_lyric();
			break;
		default:
			break;
		}
	}

}