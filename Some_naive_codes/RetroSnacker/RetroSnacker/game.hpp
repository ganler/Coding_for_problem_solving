#pragma once

#define SLEEPFOR(x) { std::this_thread::sleep_for(std::chrono::milliseconds((x))); }
#define GAME_PAUSE  80
#define COLS        700
#define ROWS        500

#include <chrono>
#include <thread>
#include <graphics.h>
#include <conio.h>
#include <thread>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include "game_map.hpp"

using namespace std;


class Information
{
public:

	void update_info()
	{
		std::string news;
		static random_device rd;
		static mt19937 engine{ rd() };
		static uniform_int_distribution<uint16_t> u(0, 9);
		static RECT news_impl = { 0, 355, 190, 450 };

		drawtext(/* �ֶ����� */
			"                                       \n                                       \n                                       \n                                       ",
			&news_impl, DT_CENTER
		);
		switch (u(engine))
		{
		case 0:
			news = "������ʡ����\n������Ϊʲô��ôǿ��\n��ά˹Ϊʲô��ô��������\n����ΰΪʲô��ô��";
			break;
		case 1:
			news = "�������̣�������nb";
			break;
		case 2:
			news = "ÿ������������\n��һ��lrznb��������";
			break;
		case 3:
			news = "�����ߴ�������Ʒ����·�";
			break;
		case 4:
			news = "������GUI����EasyX����\nΪ����Ϸ������ʹ����\ndeque�Լ�\nֻ�û��¾ɽڵ�������";
			break;
		case 5:
			news = "�ŷɰ���\n�������ӻ��ж�\nʹ���Ʒ���\n�۹��ӽ����+10��\n";
			break;
		case 6:
			news = "����Release����ģʽ\n�ɵõ�����������\n��Ϸ����Ŷ";
			break;
		case 7:
			news = "����̰�����е��������";
			break;
		case 8:
			news = "�����ʹ��awsd\nҲ����ֱ����������";
			break;
		case 9:
			news = R"(���Ʋ�����Ϸ �ܾ�������Ϸ
ע�����ұ��� ������ƭ�ϵ�
�ʶ���Ϸ���� ������Ϸ����
������ʱ�� ���ܽ�������)";
			break;
		}
		drawtext(news.c_str(), &news_impl, DT_CENTER);
	}

	void draw(int score, int highest) noexcept
	{
		LOGFONT f;
		gettextstyle(&f);
		f.lfHeight = 18.5;
		f.lfWeight = 4;
		_tcscpy_s(f.lfFaceName, _T("΢���ź�"));
		f.lfQuality = ANTIALIASED_QUALITY;
		settextstyle(&f);


		static uint64_t counter;
		RECT score_impl = { 10, 60, 190, 100 };
		drawtext((to_string(score) + " / " + to_string(highest) + " (��ʷ��߼�¼)").c_str(), &score_impl, DT_CENTER);
		if (counter++ % 20 == 0)
			update_info();
	}

	void init(mode fm) noexcept
	{
		LOGFONT f;
		gettextstyle(&f);
		f.lfHeight = 24;
		f.lfWeight = 6;
		_tcscpy_s(f.lfFaceName, _T("΢���ź�"));
		f.lfQuality = ANTIALIASED_QUALITY;
		settextstyle(&f);

		RECT score_zone = { 10, 10, 190, 50 };
		drawtext(">>> Scores", &score_zone, DT_CENTER);

		std::string level_str = ">>> Level\n\n";
		level_str += fm == mode::unknown ? "{ ��ѡ�� }" : (fm == mode::easy) ? "{ ���Ű� }" : (fm == mode::mid ? "{ ���װ� }" : (fm == mode::fly ? "{ �ŷɰ� }" :"{ �߼��� }"));

		RECT level_zone = { 10, 160, 190, 250 };
		drawtext(level_str.c_str(), &level_zone, DT_CENTER);

		RECT news_zone = { 10, 290, 190, 330 };
		drawtext(">>> News", &news_zone, DT_CENTER);
	}
};

class game
{
public:
    void run()
	{
		update_history_score();
		while (menu())
		{
			thread t([&]() {do
			{
				SLEEPFOR(GAME_PAUSE);
				draw();
			} while (gaming = m_map.snake_next_step() && !strong_ext); } );

			while (gaming)
				if (_kbhit())
					switch (_getch())
					{
					case 72: 
					case 'w':
						m_map.dir = direction::north; break;
					case 80: 
					case 's':
						m_map.dir = direction::south; break;
					case 75: 
					case 'a':
						m_map.dir = direction::west; break;
					case 77: 
					case 'd':
						m_map.dir = direction::east; break;
					case 27:
						strong_ext = true;
						break;
					default:
						break;
					}

			t.join();
			update_history_score();
		}
		
	}
public:
	~game()
	{
		closegraph();
	}
	bool menu()
	{// ѡȡģʽ
		initgraph(COLS, ROWS);

		setlinestyle(PS_SOLID, 3);
		setbkcolor(WHITE);
		settextcolor(BLACK);


		cleardevice();
		
		LOGFONT f;
		gettextstyle(&f);
		f.lfHeight = 24;
		f.lfWeight = 6;
		_tcscpy_s(f.lfFaceName, _T("΢���ź�"));
		f.lfQuality = ANTIALIASED_QUALITY;
		settextstyle(&f);
		RECT menu = { 10, 10, COLS, ROWS};
		drawtext("��ѡ��ģʽ��\n  a: ���Ű汾\n  b: ���װ汾 \n  c: �߼��汾\n  d: �ŷɰ汾\n\n [ESC]: �˳���Ϸ", &menu, DT_CENTER);
		
		

		bool not_inp = !(gmode == mode::hard && m_map.lifes < 5 && m_map.lifes > 0);


		while (not_inp)
		{
			not_inp = false;
			switch (_getch())
			{
			case 'a':
				gmode = mode::easy;
				break;
			case 'b':
				gmode = mode::mid; 
				break;
			case 'c':
				gmode = mode::hard; 
				break;
			case 'd':
				gmode = mode::fly; 
				break;
			case 27:
				quick_exit(0);
			default:
				not_inp = true;
				break;
			}
		}

		restart();
		return true;
	}
	void draw()
	{
		m_map.draw();
		history_highest_score = max(history_highest_score, m_map.scores);
		m_info.draw(m_map.scores, history_highest_score);
	}
	void restart()
	{
		
		strong_ext = false;
		history_highest_score = history[gmode];
		gaming = true;
		m_map.init(gmode);
		m_info.init(gmode);
	}
public:
	// Utils
	void update_history_score()
	{
		constexpr char filename[] = "record.txt";
		ifstream file(filename);
		if (file.is_open() && file.peek() != ifstream::traits_type::eof())
		{
			for (int i = 0; i < 4; ++i)
				file >> history[i];

			history[gmode] = max(history[gmode], history_highest_score);

			ofstream create_file;
			create_file.open(filename);
			file.clear();
			for (int i = 0; i < 4; ++i)
				create_file << history[i] << ' ';
			create_file.close();
			file.close();
		}
		else
		{
			ofstream create_file;
			create_file.open(filename);
			for (int i = 0; i < 4; ++i)
				create_file << 0 << ' ';
			create_file.close();
		}
	}

private:
	game_map     m_map;
	int          history_highest_score;
	int          history[4];
	mode         gmode;
	Information  m_info;
	bool         gaming;
	bool         strong_ext;
};