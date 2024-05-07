#include <graphics.h>
#include <string>
#include <vector>
//int idx_current_animer = 0;
const int PLAYER_ANIMER_NUM = 6;  


const int WINDOW_WIDTH = 1280;  //窗口宽度
const int WINDOW_HIGHT = 720;	//窗口高度
const int BUTTON_WIDTH = 192;   //按钮宽度
const int BUTTON_HIGHT = 75;	//按钮高度

int BULLET_NUM = 1;			  //子弹数量
bool is_game_started = false; //游戏主界面
bool running = true;		  //程序是否运行

//old
IMAGE img_player_left[PLAYER_ANIMER_NUM];
IMAGE img_player_right[PLAYER_ANIMER_NUM];



#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"MSIMG32.LIB")
inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
}
class Animation
{
public:
	Animation(LPCTSTR path, int num, int interval)
	{
		interval_ms = interval;
		TCHAR path_file[256];
		for (size_t i = 0; i < num; i++)
		{
			_stprintf_s(path_file, path, i);
			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);
			frame_list.push_back(frame);
		}
	}
	~Animation()
	{
		for (size_t i = 0; i < frame_list.size(); i++)
			delete frame_list[i];
	}

	void play(int x, int y, int delta)
	{
		timer += delta;
		if (timer >= interval_ms)
		{
			idx_frame = (idx_frame + 1) % frame_list.size();
			timer = 0;
		}
		putimage_alpha(x, y, frame_list[idx_frame]);
	}
private:
	std::vector<IMAGE*> frame_list;
	int interval_ms = 0;
	int timer = 0;//动画定时
	int idx_frame = 0; //动画帧索引
};
class Player
{
public:
	Player()
	{
		loadimage(&img_shadow, _T("img/shadow_player.png"));
		anim_left = new Animation(_T("img/player_left_%d.png"), 6, 45);
		anim_right = new Animation(_T("img/player_right_%d.png"), 6, 45);

	}
	~Player()
	{
		delete anim_left;
		delete anim_right;
	}
	const POINT& GetPosition() const
	{
		return player_pos;
	}
	void ProcessEvent(const ExMessage& msg)
	{
		switch (msg.message)
		{
		case WM_KEYDOWN:
			switch (msg.vkcode)
			{
			case VK_UP:
				//player_pos.y -= PLAYER_SPEED;
				is_moving_up = true;
				break;
			case VK_DOWN:
				//player_pos.y += PLAYER_SPEED;
				is_moving_down = true;
				break;
			case VK_LEFT:
				//player_pos.x -= PLAYER_SPEED;
				is_moving_left = true;
				break;
			case VK_RIGHT:
				//player_pos.x += PLAYER_SPEED;
				is_moving_right = true;
				break;

			}
			break;
		case WM_KEYUP: //流畅操作
			switch (msg.vkcode)
			{
			case VK_UP:
				is_moving_up = false;
				break;
			case VK_DOWN:
				is_moving_down = false;
				break;
			case VK_LEFT:
				is_moving_left = false;
				break;
			case VK_RIGHT:
				is_moving_right = false;
				break;
			}
			break;
		}
	}
	void Move()
	{

		int dir_x = is_moving_right - is_moving_left;
		int dir_y = is_moving_down - is_moving_up;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0)
		{
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			player_pos.x += (int)(PLAYER_SPEED * normalized_x);
			player_pos.y += (int)(PLAYER_SPEED * normalized_y);
		}

		if (player_pos.x < 0) player_pos.x = 0;
		if (player_pos.y < 0) player_pos.y = 0;
		if (player_pos.x + PLAYER_WIDTH > WINDOW_WIDTH) player_pos.x = WINDOW_WIDTH - PLAYER_WIDTH;
		if (player_pos.y + PLAYER_HIGHT > WINDOW_HIGHT) player_pos.y = WINDOW_HIGHT - PLAYER_HIGHT;

		if (is_moving_up) player_pos.y -= PLAYER_SPEED;
		if (is_moving_down) player_pos.y += PLAYER_SPEED;
		if (is_moving_left) player_pos.x -= PLAYER_SPEED;
		if (is_moving_right) player_pos.x += PLAYER_SPEED;

	}
	void Draw(int delta)
	{
		int player_shadow_x = player_pos.x + (PLAYER_WIDTH / 2 - SHADOW_WIDTH / 2);
		int player_shadow_y = player_pos.y + PLAYER_HIGHT - 8;
		putimage_alpha(player_shadow_x, player_shadow_y, &img_shadow); //阴影

		static bool facing_left = false;
		int dir_x = is_moving_right - is_moving_left;
		if (dir_x < 0)
			facing_left = true;
		else if (dir_x > 0)
			facing_left = false;
		if (facing_left)
			anim_left->play(player_pos.x, player_pos.y, delta);
		else
			anim_right->play(player_pos.x, player_pos.y, delta);
	}


public:
	const int PLAYER_WIDTH = 80; //玩家宽度
	const int PLAYER_HIGHT = 80; //玩家高度
private:
	const int PLAYER_SPEED = 2;  //玩家速度
	const int SHADOW_WIDTH = 32; //阴影宽度
private:
	IMAGE img_shadow;
	bool is_moving_up = false;
	bool is_moving_down = false;
	bool is_moving_left = false;
	bool is_moving_right = false;
	Animation* anim_left;
	Animation* anim_right;
	POINT player_pos = { 500,500 };

};
class Bullet
{
public:
	POINT position = { 0,0 };
public:
	Bullet() = default;
	~Bullet() = default;

	void Draw() const
	{
		setlinecolor(RGB(255, 155, 50));
		setfillcolor(RGB(200, 75, 10));
		fillcircle(position.x, position.y, RADIUS);
	}
private:
	const int RADIUS = 10;
};
class Enemy
{
public:
	Enemy()
	{
		loadimage(&img_shadow, _T("img/shadow_player.png"));
		anim_left = new Animation(_T("img/enemy_left_%d.png"), 6, 45);
		anim_right = new Animation(_T("img/enemy_right_%d.png"), 6, 45);
		//敌人生成边界
		enum class SpwnEdge
		{
			UP = 0,
			Down,
			Left,
			Right
		};
		SpwnEdge edge = (SpwnEdge)(rand() % 4);
		switch (edge)
		{
		case SpwnEdge::UP:
			position.x = rand() % WINDOW_WIDTH;
			position.y = -ENEMY_HIGHT;
			break;
		case SpwnEdge::Down:
			position.x = rand() % WINDOW_WIDTH;
			position.y = WINDOW_HIGHT;
			break;
		case SpwnEdge::Left:
			position.x = -ENEMY_WIDTH;
			position.y = rand() % WINDOW_HIGHT;
			break;
		case SpwnEdge::Right:
			position.x = ENEMY_WIDTH;
			position.y = rand() % WINDOW_HIGHT;
			break;
		default:
			break;
		}
	}
	~Enemy()
	{
		delete anim_left;
		delete anim_right;
	}

	bool CheckBulletCollision(const Bullet& bullet)
	{
		bool is_overlap_x = bullet.position.x >= position.x && bullet.position.x <= position.x + ENEMY_WIDTH;
		bool is_overlap_y = bullet.position.y >= position.y && bullet.position.y <= position.y + ENEMY_HIGHT;
		return is_overlap_x && is_overlap_y;
	}
	bool CheckPlayerCollision(const Player& player)
	{
		POINT check_position = { position.x + ENEMY_WIDTH / 2  ,position.y + ENEMY_HIGHT /2 };
		bool is_overlap_x = check_position.x>= player.GetPosition().x && check_position.x <= player.GetPosition().x + player.PLAYER_WIDTH;
		bool is_overlap_y = check_position.y >= player.GetPosition().y && check_position.y <= player.GetPosition().y + player.PLAYER_HIGHT;
		return is_overlap_x && is_overlap_y;
	}
	void Move(const Player& player)
	{
		const POINT& player_postion = player.GetPosition();
		int dir_x = player_postion.x - position.x;
		int dir_y = player_postion.y - position.y;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0)
		{
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			position.x += (int)(ENEMY_SPEED * normalized_x);
			position.y += (int)(ENEMY_SPEED * normalized_y);
		}

		if (dir_x < 0)
			facing_left = true;
		else if (dir_x > 0)
			facing_left = false;

	}
	void Draw(int delta)
	{
		int player_shadow_x = position.x + (ENEMY_WIDTH / 2 - SHADOW_WIDTH / 2);
		int player_shadow_y = position.y + ENEMY_HIGHT - 35;
		putimage_alpha(player_shadow_x, player_shadow_y, &img_shadow); //阴影

		if (facing_left)
			anim_left->play(position.x, position.y, delta);
		else
			anim_right->play(position.x, position.y, delta);
	}
	void Hurt()
	{
		alive = false;
	}
	bool CheckAlive()
	{
		return alive;
	}
private:
	const int ENEMY_SPEED = 2;  //敌人速度
	const int ENEMY_WIDTH = 80; //敌人宽度
	const int ENEMY_HIGHT = 80; //敌人高度
	const int SHADOW_WIDTH = 48; //阴影宽度
private:
	IMAGE img_shadow;
	Animation* anim_left;
	Animation* anim_right;
	POINT position = { 0,0 };
	bool facing_left = false;
	bool alive = true;
};
class Button
{
public:
	Button(RECT rect,LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
	{
		region = rect;
		loadimage(&img_idle, path_img_idle);
		loadimage(&img_hovered, path_img_hovered);
		loadimage(&img_pushed, path_img_pushed);
	}
	~Button() = default;
	void Draw()
	{
		switch (status)
		{
		case Status::Idle:
			putimage(region.left, region.top, &img_idle);
			break;
		case Status::Hovered:
			putimage(region.left, region.top, &img_hovered);
			break;
		case Status::Pushed:
			putimage(region.left, region.top, &img_pushed);
			break;
		}
	}
protected:
	virtual void OnClick() = 0;
private:
	enum class Status
	{
		Idle = 0,
		Hovered,
		Pushed
	};
private:
	RECT region;
	IMAGE img_idle;
	IMAGE img_hovered;
	IMAGE img_pushed;
	Status status = Status::Idle;
private:
	//检测鼠标点击
	bool CheckCursorHit(int x, int y) {
		return x >= region.left && x <= region.right&& y >= region.top && y <= region.bottom;
	}
public:
	void ProcessEvent(const ExMessage& msg)
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			if (status == Status::Idle && CheckCursorHit(msg.x, msg.y))
				status = Status::Hovered;
			else if(status == Status::Hovered && CheckCursorHit(msg.x, msg.y))
				status = Status::Idle;
			break;
		case WM_LBUTTONDOWN:
			if(CheckCursorHit(msg.x,msg.y))
				status = Status::Pushed;
			break;
		case WM_LBUTTONUP:
			if (status == Status::Pushed)
				OnClick();
			break;
		default:
			break;
		}
	}
};
class StartGameButton :public Button
{
public:
	StartGameButton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		:Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~StartGameButton() = default;
protected:
	void OnClick()
	{

		mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);
		is_game_started = true;
	}
};
class QuitGameButton :public Button
{
public:
	QuitGameButton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		:Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~QuitGameButton() = default;
protected:
	void OnClick()
	{
		running = false;
	}
};
//old
void LoadAnimation()
{
	for (size_t i = 0; i < PLAYER_ANIMER_NUM; i++)
	{
		std::wstring path = L"img/player_left_" + std::to_wstring(i) + L".png";
		loadimage(&img_player_left[i], path.c_str());
	}
	for (size_t i = 0; i < PLAYER_ANIMER_NUM; i++)
	{
		std::wstring path = L"img/player_right_" + std::to_wstring(i) + L".png";
		loadimage(&img_player_right[i], path.c_str());
	}

}
//Animation anim_left_player(_T("img/player_left_%d.png"), 6, 45);
//Animation anim_right_player(_T("img/player_right_%d.png"), 6, 45);
//old
void DrawPlayer(int delta, int dir_x)
{
	
}
//生成新的敌人
void TryGenerateEnemy(std::vector<Enemy*>& enemy_list)
{
	const int INTERVAL = 100;
	static int counter = 0;
	if ((++counter) % INTERVAL == 0)
	{
		enemy_list.push_back(new Enemy());
	}
}
//更新子弹
void UpdateBullets(std::vector<Bullet>& bullet_list, const Player& player)
{
	const double RADIAL_SPEED = 0.0045;
	const double TANGENT_SPEED = 0.0055;
	double radian_interval = 2 * 3.14159 / bullet_list.size();
	POINT player_position = player.GetPosition();
	double radius = 100 + 25 * sin(GetTickCount() * RADIAL_SPEED);
	for (size_t i = 0; i < bullet_list.size(); i++)
	{
		double radian = GetTickCount() * TANGENT_SPEED + radian_interval * i;
		bullet_list[i].position.x = player_position.x + player.PLAYER_WIDTH / 2 + (int)(radius * sin(radian));
		bullet_list[i].position.y = player_position.y + player.PLAYER_HIGHT/ 2 + (int)(radius * cos(radian));
	}
}
//左上角得分
void DrawPlayerScore(int score)
{
	static TCHAR text[64];
	_stprintf_s(text, _T("当前玩家得分：%d"), score);
	setbkmode(TRANSPARENT);
	settextcolor(RGB(255, 85, 185));
	outtextxy(10, 10, text);
}



//序列帧动画
int main()
{
	initgraph(WINDOW_WIDTH, WINDOW_HIGHT);
	mciSendString(_T("open mus/bgm.mp3 alias bgm"), NULL, 0, NULL);
	mciSendString(_T("open mus/hit.wav alias hit"), NULL, 0, NULL);




	int score = 0;

	ExMessage msg;
	IMAGE img_background;
	IMAGE img_menu;
	Player player;
	std::vector<Enemy*> enemy_list;
	std::vector<Bullet> bullet_list(BULLET_NUM);
	RECT region_btn_start_game, region_btn_quit_game;
	region_btn_start_game.left = (WINDOW_WIDTH - BUTTON_WIDTH)/2;
	region_btn_start_game.right = region_btn_start_game.left+BUTTON_WIDTH;
	region_btn_start_game.top = 430;
	region_btn_start_game.bottom = region_btn_start_game.top +BUTTON_HIGHT;

	region_btn_quit_game.left = (WINDOW_WIDTH - BUTTON_WIDTH)/2;
	region_btn_quit_game.right = region_btn_quit_game.left + BUTTON_WIDTH;
	region_btn_quit_game.top = 550;
	region_btn_quit_game.bottom = region_btn_quit_game.top + BUTTON_HIGHT;

	StartGameButton btn_start_game = StartGameButton(region_btn_start_game,
		_T("img/ui_start_idle.png"), _T("img/ui_start_hovered.png"), _T("img/ui_start_pushed.png"));
	QuitGameButton btn_quit_game = QuitGameButton(region_btn_quit_game,
		_T("img/ui_quit_idle.png"), _T("img/ui_quit_hovered.png"), _T("img/ui_quit_pushed.png"));


	loadimage(&img_background, _T("img/background.png"));
	loadimage(&img_menu, _T("img/menu.png"));

	//LoadAnimation();
	BeginBatchDraw();
	while (running)
	{
		DWORD start_time = GetTickCount();
		
		while (peekmessage(&msg))
		{
			if(is_game_started)
				player.ProcessEvent(msg);
			else
			{
				btn_start_game.ProcessEvent(msg);
				btn_quit_game.ProcessEvent(msg);
			}
		}
		if (is_game_started)
		{
			player.Move();
			TryGenerateEnemy(enemy_list);
			UpdateBullets(bullet_list, player);
			for (Enemy* enemy : enemy_list)
				enemy->Move(player);
			for (Enemy* enemy : enemy_list)
			{
				if (enemy->CheckPlayerCollision(player))
				{
					MessageBox(GetHWnd(), _T("菜就多练"), _T("game over"), MB_OK);
					static TCHAR text[128];
					_stprintf_s(text, _T("最终得分:%d"), score);
					MessageBox(GetHWnd(), text, _T("game over"), MB_OK);
					running = false;
					break;
				}
			}
			for (Enemy* enemy : enemy_list)
			{
				for (const Bullet& bullet : bullet_list)
				{
					if (enemy->CheckBulletCollision(bullet))
					{
						mciSendString(_T("play hit from 0"), NULL, 0, NULL);
						enemy->Hurt();
						score++;
						if (score % 10 == 0)
						{
							Bullet newBullet;
							bullet_list.push_back(newBullet);
						}
					}
				}
			}
			for (size_t i = 0; i < enemy_list.size(); i++)
			{
				Enemy* enemy = enemy_list[i];
				if (!enemy->CheckAlive())
				{
					std::swap(enemy_list[i], enemy_list.back());
					enemy_list.pop_back();
					delete enemy;
				}
			}
		}
		//static int counter = 0;
		//if (++counter % 5 == 0)
		//	idx_current_animer++;
		////使动画循环播放
		//idx_current_animer = idx_current_animer % PLAYER_ANIMER_NUM;

		cleardevice();
		if (is_game_started)
		{
			putimage(0, 0, &img_background);//0,0放置图片
			player.Draw(1000 / 144);
			for (Enemy* enemy : enemy_list)
				enemy->Draw(1000 / 144);
			for (const Bullet& bullet : bullet_list)
				bullet.Draw();
			DrawPlayerScore(score);
		}
		else
		{
			putimage(0, 0, &img_menu);
			btn_start_game.Draw();
			btn_quit_game.Draw();
		}


		FlushBatchDraw();

		DWORD end_time = GetTickCount();
		DWORD delta_time = start_time - end_time;
		if (delta_time < 1000 / 144)
		{
			Sleep(1000 / 144 - delta_time);
		}
	}
}