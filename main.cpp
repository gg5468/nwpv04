#include "nwpwin.h"

class ship : public vsite::nwp::window
{
public:
	std::string class_name() override {
		return "STATIC";
	}
	POINT p;

	int size;
	bool is_moving = false;
	void set_is_moving(bool flag) {
		DWORD style;
		if (flag) {
			style = WS_CHILD | WS_VISIBLE;
		}
		else {
			style = WS_CHILD | WS_VISIBLE | WS_BORDER;
		}
		SetWindowLong(*this, GWL_STYLE, style);
		SetWindowPos(*this, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

		is_moving = flag;
	}
	void move(int vk, RECT parent_rect)
	{
		RECT r;
		GetClientRect(*this, &r);
		int speed = GetAsyncKeyState(VK_CONTROL) ? 50 : 10;

		switch (vk)
		{
		case(VK_UP):
			p.y = max(p.y - speed, parent_rect.top);
			break;
		case(VK_DOWN):
			p.y = min(p.y + speed, parent_rect.bottom - size);
			break;
		case(VK_LEFT):
			p.x = max(p.x - speed, parent_rect.left);
			break;
		case(VK_RIGHT):
			p.x = min(p.x + speed, parent_rect.right - size);
			break;
		}
		
		SetWindowPos(*this, 0, p.x, p.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}
};

class main_window : public vsite::nwp::window
{
protected:
	void on_left_button_down(POINT p) override {
		s.size = 20;
		s.p = p;

		if (!s)
		{
			s.create(*this, WS_CHILD | WS_VISIBLE | WS_BORDER, " X", 0, p.x, p.y, s.size, s.size);
		}
		else 
		{
			SetWindowPos(s, 0, p.x, p.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}
	}
	bool is_arrow_key(int vk) {
		return vk == VK_UP || vk == VK_DOWN || vk == VK_RIGHT || vk == VK_LEFT;
	}
	void on_key_up(int vk) override {
		if (is_arrow_key(vk) && s) {
			s.set_is_moving(false);
		}
	}
	void on_key_down(int vk) override {
		if (is_arrow_key(vk) && s) {
			s.set_is_moving(true);

			RECT parent_rect;
			GetClientRect(*this, &parent_rect);
			
			s.move(vk, parent_rect);
		}
	}
	void on_destroy() override {
		::PostQuitMessage(0);
	}
private:
	ship s;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hp, LPSTR cmdLine, int nShow)
{
	vsite::nwp::application app;
	main_window w;
	w.create(0, WS_OVERLAPPEDWINDOW | WS_VISIBLE, "NWP 4");
	return app.run();
}
