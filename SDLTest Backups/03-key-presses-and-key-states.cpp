#include<SDL3/SDL.h>
#include<SDL3/SDL_main.h>
#include<SDL3_image/SDL_image.h>
#include<string>

class LTexture {
public:
	LTexture();

	~LTexture();

	bool loadFromFile(std::string path);

	void destroy();

	void render(float x, float y);

	int getWidth();
	int getHeight();
	bool isLoaded();

private:
	SDL_Texture* mTexture;
	int mWidth;
	int mHeight;
};


constexpr int kScreenWidth{ 640 };
constexpr int kScreenHeight{ 480 };

SDL_Window* gWindow{ nullptr };
SDL_Renderer* gRenderer{ nullptr };
LTexture gUpTexture, gDownTexture, gLeftTexture, gRightTexture;

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
			SDL_Log("Unable to create texture from loaded pixels! SDL Error: %s\m", SDL_GetError());
		}
		else {
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}


		SDL_DestroySurface(loadedSurface);
	}

	return mTexture != nullptr;
}

void LTexture::destroy() {
	SDL_DestroyTexture(mTexture);
	mTexture = nullptr;
	mWidth = 0;
	mHeight = 0;
}

void LTexture::render(float x, float y) {
	SDL_FRect dstRect{ x, y, static_cast<float>(mWidth), static_cast<float>(mHeight) };
	SDL_RenderTexture(gRenderer, mTexture, nullptr, &dstRect);
}

int LTexture::getWidth(){
	return mWidth;
}

int LTexture::getHeight() {
	return mHeight;
}

bool LTexture::isLoaded() {
	return mTexture != nullptr;
}

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
		if (SDL_CreateWindowAndRenderer("SDL4 Tutorial: Key Presses and Key States", kScreenWidth, kScreenHeight, 0, &gWindow, &gRenderer) == false) {
			SDL_Log("Window could not be created!SDL Error: %s\n", SDL_GetError());
			success = false;
		}
	}

	return success;
}

bool loadMedia() {
	bool success{ true };

	if (gUpTexture.loadFromFile("03-key-presses-and-key-states/up.png") == false) {
		SDL_Log("Unable to load images\n");
		success = false; 
	}
	if (gDownTexture.loadFromFile("03-key-presses-and-key-states/down.png") == false) {
		SDL_Log("Unable to load images\n");
		success = false;
	}
	if (gLeftTexture.loadFromFile("03-key-presses-and-key-states/left.png") == false) {
		SDL_Log("Unable to load images\n");
		success = false;
	}
	if (gRightTexture.loadFromFile("03-key-presses-and-key-states/right.png") == false) {
		SDL_Log("Unable to load images\n");
		success = false;
	}

	return success;
}

void close() {
	SDL_DestroyRenderer(gRenderer);
	gRenderer = nullptr;
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;

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

			LTexture* currentTexture = &gUpTexture; //Adress of current texture = Adress of gUpTexture, to avoid creating copy
			SDL_Color bgColor{ 0xFF, 0xFF, 0xFF, 0xFF };

			while (quit == false) {
				while (SDL_PollEvent(&e) == true) {
					if (e.type == SDL_EVENT_QUIT) {
						quit = true;
					}
					else if (e.type == SDL_EVENT_KEY_DOWN) {
						if (e.key.key == SDLK_UP) {
							currentTexture = &gUpTexture;
						}
						else if (e.key.key == SDLK_DOWN) {
							currentTexture = &gDownTexture;
						}
						else if (e.key.key == SDLK_LEFT) {
							currentTexture = &gLeftTexture;
						}
						else if (e.key.key == SDLK_RIGHT) {
							currentTexture = &gRightTexture;
						}
					}
				}

				bgColor.r = 0xFF;
				bgColor.g = 0xFF;
				bgColor.b = 0xFF;

				const bool* keyStates = SDL_GetKeyboardState(nullptr);
				if (keyStates[SDL_SCANCODE_UP] == true) {
					bgColor.r = 0xFF;
					bgColor.g = 0x00;
					bgColor.b = 0x00;
				}
				else if (keyStates[SDL_SCANCODE_DOWN] == true) {
					bgColor.r = 0x00;
					bgColor.g = 0xFF;
					bgColor.b = 0x00;
				}
				else if (keyStates[SDL_SCANCODE_LEFT] == true) {
					bgColor.r = 0xFF;
					bgColor.g = 0xFF;
					bgColor.b = 0x00;
				}
				else if (keyStates[SDL_SCANCODE_RIGHT] == true) {
					bgColor.r = 0x00;
					bgColor.g = 0x00;
					bgColor.b = 0xFF;
				}

				SDL_SetRenderDrawColor(gRenderer, bgColor.r, bgColor.g, bgColor.b, 0xFF);
				SDL_RenderClear(gRenderer);

				currentTexture->render((kScreenWidth - currentTexture->getWidth()) / 2.f, (kScreenHeight - currentTexture->getHeight()) / 2.f);

				SDL_RenderPresent(gRenderer);
			}
		}

	}

	close();

	return exitCode;

}