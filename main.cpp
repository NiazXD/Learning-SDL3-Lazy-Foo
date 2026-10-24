#include<SDL3/SDL.h>
#include<SDL3/SDL_main.h>
#include<SDL3_image/SDL_image.h>
#include<string>

constexpr int kScreenWidth{ 1280 };
constexpr int kScreenHeight{ 720 };

SDL_Window* gWindow;
SDL_Renderer* gRenderer;

class LTexture {
private:
	SDL_Texture* mTexture;
	int mWidth;
	int mHeight;

public:
	LTexture() :
		mTexture{ nullptr },
		mWidth {0},
		mHeight {0}
	{

	}

	~LTexture() 
	{
		Destroy();
	}

	bool LoadFromFile(std::string path) 
	{
		SDL_Surface* loadedSurface = IMG_Load(path.c_str());
		mTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		mWidth = loadedSurface->w;
		mHeight = loadedSurface->h;
		SDL_DestroySurface(loadedSurface);
		return mTexture != nullptr;
	}

	void Destroy()
	{
		SDL_DestroyTexture(mTexture);
		mTexture = nullptr;
		mWidth = 0;
		mHeight = 0;
	}

	void Render(float x, float y) {
		SDL_FRect dstRect{ x, y, static_cast<float>(mWidth), static_cast<float>(mHeight) };
		SDL_RenderTexture(gRenderer, mTexture, nullptr, &dstRect);
	}

	int GetWidth() {
		return mWidth;
	}

	int GetHeight() {
		return mHeight;
	}
};

LTexture gTexture;

bool Init()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer("Maheen bb", kScreenWidth, kScreenHeight, 0, &gWindow, &gRenderer);;
	return(gWindow != nullptr && gRenderer != nullptr);
}

bool LoadMedia() {
	return gTexture.LoadFromFile("TestImages/Maheen.png");
}

void close() {
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;
	SDL_DestroyRenderer(gRenderer);
	gRenderer = nullptr;

	SDL_Quit();
}

int main(int argc, char* args[]) {

	if (!Init())
		return 0;

	if (!LoadMedia())
		return 0;


	bool quit{ false };
	SDL_Event e;
	SDL_zero(e);

	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_EVENT_QUIT) 
				quit = true;
		}

		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);
		gTexture.Render((kScreenWidth - gTexture.GetWidth()) / 2.f, (kScreenHeight - gTexture.GetHeight()) / 2.f);
		SDL_RenderPresent(gRenderer);
	}

	close();

	return 0;
}

