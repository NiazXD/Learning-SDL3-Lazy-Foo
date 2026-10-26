#include<SDL3/SDL.h>
#include<SDL3/SDL_main.h>
#include<SDL3_image/SDL_image.h>
#include<SDL3_ttf/SDL_ttf.h>
#include<string>
#include<sstream>


class LTexture {
public:

	static constexpr float kOriginalSize = -1.f;

	LTexture();

	~LTexture();

	bool loadFromFile(std::string path);

#if defined(SDL_TTF_MAJOR_VERSION)
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
#endif

	void destroy();

	void setColor(Uint8 r, Uint8 g, Uint8 b);

	void setAlpha(Uint8 alpha);

	void setBlending(SDL_BlendMode blendMode);

	void render(float x, float y, SDL_FRect* clip = nullptr, float width = kOriginalSize, float height = kOriginalSize,
		double degrees = 0.0, SDL_FPoint* center = nullptr, SDL_FlipMode flipMode = SDL_FLIP_NONE);

	int getWidth();
	int getHeight();
	bool isLoaded();

private:
	SDL_Texture* mTexture;
	int mWidth;
	int mHeight;
};

class LButton {
public:
	static constexpr int kButtonWidth = 300;
	static constexpr int kButtonHeight = 200;

	LButton();

	void setPosition(float x, float y);
	
	void handleEvent(SDL_Event* e);

	void render();

private:
	enum class eButtonSprite {
		MouseOut = 0,
		MouseOverMotion = 1,
		MouseDown = 2,
		MouseUp = 3
	};

	SDL_FPoint mPosition;

	eButtonSprite mCurrentSprite;
};


class LTimer 
{
public:
	LTimer();

	void start();
	void stop();
	void pause();
	void unpause();

	Uint64 getTicksNS();

	bool isStarted();
	bool isPaused();
	
private:
	Uint64 mStartTicks;

	Uint64 mPausedTicks;

	bool mPaused;
	bool mStarted;

};



constexpr int kScreenWidth{ 640 };
constexpr int kScreenHeight{ 480 };

SDL_Window* gWindow{ nullptr };
SDL_Renderer* gRenderer{ nullptr };
TTF_Font* gFont{ nullptr };
LTexture gColorsTexture, gTextTexture, gButtonSpriteTexture, gTimeTextTexture;

LTexture::LTexture() :
	mTexture{ nullptr },
	mWidth{ 0 },
	mHeight{ 0 }
{

}

LTexture::~LTexture() {
	destroy();
}

bool LTexture::loadFromFile(std::string path) {
	destroy();

	if (SDL_Surface* loadedSurface = IMG_Load(path.c_str()); loadedSurface == nullptr) {
		SDL_Log("Unable to load Image! SDL Error: %s\n", SDL_GetError());
	}
	else {
			if (mTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface); mTexture == nullptr) {
				SDL_Log("Unable to create texture from loaded pixels! SDL Error: %s\n", SDL_GetError());
			}
			else {
				mWidth = loadedSurface->w;
				mHeight = loadedSurface->h;
			}

		SDL_DestroySurface(loadedSurface);
	}

	return mTexture != nullptr;
}

#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor) {
	destroy();

	if (SDL_Surface* textSurface = TTF_RenderText_Blended(gFont, textureText.c_str(), 0, textColor); textSurface == nullptr) {
		SDL_Log("Unable to render text surface! SDL error: %s\n", SDL_GetError());
	}
	else {
		if (mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface); mTexture == nullptr) {
			SDL_Log("Unable to create texture from rendered surface! SDL error: %s\n", SDL_GetError());
		}
		else {
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		SDL_DestroySurface(textSurface);

	}

	return mTexture != nullptr;
}
#endif


void LTexture::destroy() {
	SDL_DestroyTexture(mTexture);
	mTexture = nullptr;
	mWidth = 0;
	mHeight = 0;
}

void LTexture::setColor(Uint8 r, Uint8 g, Uint8 b) {
	SDL_SetTextureColorMod(mTexture, r, g, b);
}

void LTexture::setAlpha(Uint8 alpha) {
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::setBlending(SDL_BlendMode blendMode) {
	SDL_SetTextureBlendMode(mTexture, blendMode);
}

void LTexture::render(float x, float y, SDL_FRect* clip, float width, float height, double degrees, SDL_FPoint* center, SDL_FlipMode flipMode) {
	SDL_FRect dstRect{ x, y, static_cast<float>(mWidth), static_cast<float>(mHeight) };


	if (clip != nullptr)
	{
		dstRect.w = clip->w;
		dstRect.h = clip->h;
	}

	if (width > 0)
	{
		dstRect.w = width;
	}
	if (height > 0)
	{
		dstRect.h = height;
	}


	SDL_RenderTextureRotated(gRenderer, mTexture, clip, &dstRect, degrees, center, flipMode);
}

int LTexture::getWidth() {
	return mWidth;
}

int LTexture::getHeight() {
	return mHeight;
}

bool LTexture::isLoaded() {
	return mTexture != nullptr;
}


LButton::LButton() :
	mPosition{ 0.f, 0.f },
	mCurrentSprite{ eButtonSprite::MouseOut }
{

}

void LButton::setPosition(float x, float y)
{
	mPosition.x = x;
	mPosition.y = y;
}


void LButton::handleEvent(SDL_Event* e)
{
	if (e->type == SDL_EVENT_MOUSE_MOTION || e->type == SDL_EVENT_MOUSE_BUTTON_DOWN || e->type == SDL_EVENT_MOUSE_BUTTON_UP)
	{
		
		float x = -1.f, y = -1.f;
		SDL_GetMouseState(&x, &y);

		
		bool inside = true;

		if (x < mPosition.x)
		{
			inside = false;
		}
		else if (x > mPosition.x + kButtonWidth)
		{
			inside = false;
		}
		else if (y < mPosition.y)
		{
			inside = false;
		}
		else if (y > mPosition.y + kButtonHeight)
		{
			inside = false;
		}

		if (!inside)
		{
			mCurrentSprite = eButtonSprite::MouseOut;
		}
		//Mouse is inside button
		else
		{
			//Set mouse over sprite
			switch (e->type)
			{
			case SDL_EVENT_MOUSE_MOTION:
				mCurrentSprite = eButtonSprite::MouseOverMotion;
				break;

			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				mCurrentSprite = eButtonSprite::MouseDown;
				break;

			case SDL_EVENT_MOUSE_BUTTON_UP:
				mCurrentSprite = eButtonSprite::MouseUp;
				break;
			}
		}
	}
}

void LButton::render()
{
	SDL_FRect spriteClips[] = {
		{ 0.f, 0 * kButtonHeight, kButtonWidth, kButtonHeight },
		{ 0.f, 1 * kButtonHeight, kButtonWidth, kButtonHeight },
		{ 0.f, 2 * kButtonHeight, kButtonWidth, kButtonHeight },
		{ 0.f, 3 * kButtonHeight, kButtonWidth, kButtonHeight },
	};


	gButtonSpriteTexture.render(mPosition.x, mPosition.y, &spriteClips[static_cast<int>(mCurrentSprite)]);
}

LTimer::LTimer() :
	mStartTicks{ 0 },
	mPausedTicks{ 0 },

	mPaused{ false },
	mStarted{ false }
{

}

void LTimer::start() {
	mStarted = true;
	mPaused = false;

	mStartTicks = SDL_GetTicksNS();
	mPausedTicks = 0;
}

void LTimer::stop() {
	mStarted = false;
	mPaused = false;

	mStartTicks = 0;
	mPausedTicks = 0;
}

void LTimer::pause() {
	if (mStarted && !mPaused) {
		mPaused = true;

		mPausedTicks = SDL_GetTicksNS() - mStartTicks;
		mStartTicks = 0;
	}
}

void LTimer::unpause() {
	if (mStarted && mPaused) {
		mPaused = false;

		mStartTicks = SDL_GetTicksNS() - mPausedTicks;
		mPausedTicks = 0;
	}
}

Uint64 LTimer::getTicksNS(){ 
	Uint64 time{ 0 };

	if (mStarted) {
		if (mPaused) {
			time = mPausedTicks;
		}
		else {
			time = SDL_GetTicksNS() - mStartTicks;
		}
	}
	return time;
}

bool LTimer::isStarted() {
	return mStarted;
}

bool LTimer::isPaused() {
	return mPaused;
}





constexpr int kColorMagnitudeCount = 3;
constexpr Uint8 kColorMagnitudes[kColorMagnitudeCount] = { 0x00, 0x7f, 0xFF }; //0, half or full
enum class eColorChannel {
	TextureRed = 0,
	TextureGreen = 1,
	TextureBlue = 2,
	TextureAlpha = 3,

	BackgroundRed = 4,
	BackgroundGreen = 5,
	BackgroundBlue = 6,

	Total = 7,
	Unknown = 8
};


bool init();
bool loadMedia();
void clear();


bool init() {
	bool success{ true };

	if (SDL_Init(SDL_INIT_VIDEO) == false) {
		SDL_Log("SDL could not be iniitalized! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else {
		if (SDL_CreateWindowAndRenderer("SDL3 Tutorial: True Type Font", kScreenWidth, kScreenHeight, 0, &gWindow, &gRenderer) == false) {
			SDL_Log("Window could not be created!SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else {
			if (TTF_Init() == false) {
				SDL_Log("SDL_ttf could not initialize! SDL_ttf error: %s\n", SDL_GetError());
				success = false;
			}
		}
	}

	return success;
}

bool loadMedia() {
	bool success{ true };

	std::string fontPath{ "08-true-type-fonts/lazy.ttf" };
	if (gFont = TTF_OpenFont(fontPath.c_str(), 28); gFont == nullptr)
	{
		SDL_Log("Could not load %s! SDL_ttf Error: %s\n", fontPath.c_str(), SDL_GetError());
		success = false;
	}
	else
	{
		SDL_Color textColor{ 0x00, 0x00, 0x00, 0xFF };
		if (!gTimeTextTexture.loadFromRenderedText("Press enter to start the timer", textColor))
		{
			SDL_Log("Could not load text texture %s! SDL_ttf Error: %s\n", fontPath.c_str(), SDL_GetError());
			success = false;
		}
	}

	

	

	return success;

}

void close()
{
	//Clean up textures
	gTimeTextTexture.destroy();

	//Free font
	TTF_CloseFont(gFont);
	gFont = nullptr;

	//Destroy window
	SDL_DestroyRenderer(gRenderer);
	gRenderer = nullptr;
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;

	//Quit SDL subsystems
	TTF_Quit();
	SDL_Quit();
}




int main(int argc, char* args[]) {
	int exitCode{ 0 };

	if (init() == false) {
		SDL_Log("Initialization failed\n");
		exitCode = 1;
	}
	else {
		if (loadMedia() == false) {
			SDL_Log("Unable to load media\n");
			exitCode = 2;
		}
		else {
			bool quit{ false };
			SDL_Event e;
			SDL_zero(e);

			LTimer timer;
			std::stringstream timeText;

			while (quit == false) {
				while (SDL_PollEvent(&e) == true) {
					if (e.type == SDL_EVENT_QUIT) {
						quit = true;
					}
					else if (e.type == SDL_EVENT_KEY_DOWN)
					{
						if (e.key.key == SDLK_RETURN) {
							if (timer.isStarted()) {
								timer.stop();
							}
							else {
								timer.start();
							}
						}
						else if (e.key.key == SDLK_SPACE) {
							if (timer.isPaused()) {
								timer.unpause();
							}
							else {
								timer.pause();
							}
						}


					}
				}


				timeText.str("");
				timeText<< "Milliseconds since start time " << (timer.getTicksNS() / 1000000);
				SDL_Color textColor{ 0x00, 0x00, 0x00, 0xFF };
				gTimeTextTexture.loadFromRenderedText(timeText.str().c_str(), textColor);

				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				gTimeTextTexture.render((kScreenWidth - gTimeTextTexture.getWidth()) / 2.f, (kScreenHeight - gTimeTextTexture.getHeight()) / 2.f);

				SDL_RenderPresent(gRenderer);

			}
		}
	}


	close();

	return exitCode;

}