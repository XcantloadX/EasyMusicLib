#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "easymusic.h"

#define EXAMPLE_MP3_FILE "D:\\����MarBlue - Ȩ�����£�Cover ��������.mp3"
#define EXAMPLE_NETWORK_FILE "http://music.163.com/song/media/outer/url?id=1826139261.mp3"

//��򵥵�����
void hello_music()
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
	player_close(p);
}

//���Ӹ߼�һ��� UI
void advanced_ui()
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

		putchar('\r'); //�ƶ������굽��ͷ���������ܴ�ͷ��ʼ�������ֱ�Ӹ��ǵ�֮ǰ������
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

//��򵥵ĸ����ʾ����
int hello_lyric()
{
	const char lrc[] = "[00:00]����Marblue - Ȩ������\n[00:00]���ʣ�ST\n[00:00]�������ڹ�\n[00:22]����ĩ���̲���\n[00:24]�����ҳ�Ұ��\n[00:25]����Ю���������\n[00:27]�Ὥ��־�ھ���\n[00:29]����ҵ�и���\n[00:30]��������Խ����\n[00:33]�����¼��䴺��\n[00:34]�ȶ�������ԭ\n[00:35]ˮʦ������������\n[00:38]�����۷�����\n[00:40]�ᾣ����ɽԽ\n[00:41]��������������\n[00:53]�Ϸ��ױ�ɫ����\n[00:55]���ͻ��л���\n[00:56]��ʶ������˭����\n[00:58]�����°��֮��\n[01:00]ѡ�ͳ����ܽ�\n[01:01]���������꾡����\n[01:04]ǧ������ѻ���\n[01:05]�ʹŽ�������\n[01:07]����ҫ��ʷ������\n[01:09]Ц̸�������Ѿ�\n[01:10]���������̾\n[01:12]���Ӹõ�������ı\n[01:14]����Ӣ�ۼ�����\n[01:16]��ı��ı���𰵶�\n[01:18]����һ�����˶���\n[01:21]�����������\n[01:22]��Ұ����������\n[01:24]Ҳ����������\n[01:25]�����񳤽�����ս\n[01:26]����־�콭��\n[01:27]���������ݶ���\n[01:29]��������ӢѪ\n[01:30]չΰҵ����ָ����\n[01:31]�����ڹ����\n[01:33]����ı���Ƶ��\n[01:34]ɳ����̫ʷ��\n[01:35]������һ�ﵱʮǧ\n[01:36]��ʹ��ǧ����\n[01:38]��į�͹µ����\n[01:39]�����չڵ���\n[01:40]�����ϳɰ�ҲЦ��\n[01:53]���ͳ��ѹ�����\n[01:55]������Կձ�\n[01:56]���ǧ��ʦ������\n[01:58]���ƺ��۹����\n[02:00]ߪ³��Ȱ����\n[02:01]���ͳɰ�ҵ������\n[02:03]�̽���ʩ�¶���\n[02:05]���Ķ����հ�\n[02:06]������ǿ���ظ���\n[02:08]��Զ��δ�����\n[02:10]�����������\n[02:11]�о�Զ�ɺ�פ����\n[02:14]��ԫ�¿�ľ����\n[02:15]�뵱����Ǳ�\n[02:17]�ر�������������\n[02:19]ƾ���кῴ�⹳\n[02:21]̾������ʱ��\n[02:22]���¹�һͳû���\n[02:24]�Ƶ���׳���Գ�\n[02:26]ȴ�����Թ°�\n[02:27]ͬ����ٸ����ö�\n[02:29]ǧ�����ͽ��\n[02:31]�ٺ�Ѱ������\n[02:32]��ʿ�ۻ���������\n[02:34]����Ӣ�ۼ�����\n[02:37]��ı��ı���𰵶�\n[02:39]����һ�����˶���\n[02:41]������������\n[02:43]̤���۰��¶ɴ�\n[02:44]Ҳ����½����\n[02:45]����Ӫ����ϳ���\n[02:47]���ƺ�ѡ�ͳ�\n[02:48]���ܽ���ı��ɽ\n[02:49]������������\n[02:50]�����ϱ�����鰸\n[02:52]�ᾣ�ݿ�������\n[02:53]�ܲٹ����差\n[02:54]��������½ѷ\n[02:55]�˵о�\n[02:56]�������\n[02:57]�̽��ӳɵ�ҵ��\n[02:58]�����չ�����\n[03:00]������ǧ�ع���\n[03:01]���߶���Ц̸��\n[03:13]�������������\n[03:14]Ұ����������\n[03:15]Ҳ������������\n[03:17]���񳤽�����ս\n[03:18]����־�콭��\n[03:19]���������ݶ���\n[03:20]��������ӢѪչ\n[03:22]ΰҵ��ָ����\n[03:23]������������\n[03:24]̤���۰��¶ɴ�\n[03:25]Ҳ����½����\n[03:27]����Ӫ����ϳ���\n[03:28]���ƺ�ѡ�ͳ���\n[03:29]�ܽ���ı��ɽ\n[03:31]������������\n[03:32]�����ϱ�����鰸\n[03:33]��ʹ��ǧ����\n[03:34]��į�͹µ����\n[03:36]�����չڵ���\n[03:37]�����ϳɰ�ҲЦ��\n[03:39]�̽��ӳɵ�ҵ��\n[03:40]�����չ�����\n[03:41]������ǧ��\n[03:42]��������\n[03:43]����Ц̸��";
	Lyric *lyric = lyric_create_from_string(lrc); //�����ǵ��ַ����ﴴ�����

	EM_ERR err = 0;
	MusicPlayer *p = player_create();
	err = player_open(p, EXAMPLE_MP3_FILE);
	player_play(p);
	//������һ���ĳ�ʼ������

	char *current_line = NULL;
	while (player_status_playing_(p))
	{
		putchar('\r'); //�ƶ������굽��ͷ���������ܴ�ͷ��ʼ�������ֱ�Ӹ��ǵ�֮ǰ������
		lyric_update(lyric, player_position_get_(p), &current_line); //��ȡ����Ӧ����ʾ�ĸ���ı���current_line �� char*��һ��ָ�룩����ô &current_line ���� char**������ָ�룩
		printf("%s", current_line);
		printf("                                 "); //�ÿո񸲸ǵ���������ݡ����������һ�����һ��̵Ļ��ͻ�� bug
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
		puts("��ʾ���������");
		puts("1. hello_music��Hello World����򵥵����ӡ�");
		puts("2. play_until_end�������ط���һ�����֡�");
		puts("3. simple_control����ͣ�����š�ǰ��������");
		puts("4. advanced_ui�����߼����桰");
		puts("5. hello_network�����뻥������");
		puts("6. hello_lyric�������ʾ");
		printf("������ѡ�");
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