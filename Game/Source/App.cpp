#include "App.h"
#include "Window.h"
#include "Input.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Scene.h"
#include "Debug.h"
#include "Parallax.h"
#include "Map.h"
#include "ModuleUI.h"
#include "Collisions.h"
#include "Player.h"
#include "Pathfinding.h"
#include "Entities.h"
#include "GuiManager.h"

#include "Optick/include/optick.h"

#include "Defs.h"
#include "Log.h"

// Constructor
App::App(int argc, char* args[]) : argc(argc), args(args)
{
	PERF_START(ptimer);

	frames = 0;

	input = new Input();
	win = new Window();
	render = new Render();
	tex = new Textures();
	audio = new Audio();
	scene = new Scene();
	map = new Map();
	debug = new Debug();
	collisions = new Collisions();
	parallax = new Parallax();
	ui = new ModuleUI();
	pathfinding = new PathFinding();
	entities = new Entities();
	guimanager = new GuiManager();

	// Ordered for awake/Start/Update
	// Reverse order of CleanUp
	AddModule(input);
	AddModule(win);
	AddModule(tex);
	AddModule(audio);
	AddModule(parallax);
	AddModule(entities);
	AddModule(map);
	AddModule(ui);
	AddModule(collisions);
	AddModule(pathfinding);
	AddModule(debug);
	AddModule(scene);
	AddModule(guimanager);

	// Render last to swap buffer
	AddModule(render);

	PERF_PEEK(ptimer);
}

// Destructor
App::~App()
{
	// Release modules
	ListItem<Module*>* item = modules.end;

	while(item != NULL)
	{
		RELEASE(item->data);
		item = item->prev;
	}

	modules.Clear();

	configFile.reset();
}

void App::AddModule(Module* module)
{
	module->Init();
	modules.Add(module);
}

// Called before render is available
bool App::Awake()
{
	PERF_START(ptimer);

	// L01: DONE 3: Load config from XML
	bool ret = LoadConfig();

	if(ret == true)
	{
		// L01: DONE 4: Read the title from the config file
		title.Create(configApp.child("title").child_value());
		saveFileName = configApp.child("savefile").attribute("path").as_string();
		win->SetTitle(title.GetString());

		int cap = configApp.attribute("framerate_cap").as_int(-1);

		if (cap > 0) cappedMs = 1000.0f / cap;

		ListItem<Module*>* item;
		item = modules.start;

		while(item != NULL && ret == true)
		{
			// L01: DONE 5: Add a new argument to the Awake method to receive a pointer to an xml node.
			// If the section with the module name exists in config.xml, fill the pointer with the valid xml_node
			// that can be used to read all variables for that module.
			// Send nullptr if the node does not exist in config.xml

			ret = item->data->Awake(config.child(item->data->name.GetString()));
			item = item->next;
		}
	}

	PERF_PEEK(ptimer);

	return ret;
}

// Called before the first frame
bool App::Start()
{
	PERF_START(ptimer);

	bool ret = true;
	ListItem<Module*>* item;
	item = modules.start;

	while(item != NULL && ret == true)
	{
		ret = item->data->Start();
		item = item->next;
	}

	PERF_PEEK(ptimer);

	return ret;
}

// Called each loop iteration
bool App::Update()
{
	bool ret = true;

	PrepareUpdate();

	if(input->GetWindowEvent(WE_QUIT) == true)
		ret = false;

	if(ret == true)
		ret = PreUpdate();

	if(ret == true)
		ret = DoUpdate();

	if(ret == true)
		ret = PostUpdate();

	FinishUpdate();

	return ret;
}

// Load config from XML file
bool App::LoadConfig()
{
	bool ret = true;

	pugi::xml_parse_result result = configFile.load_file("config.xml");

	if(result == NULL)
	{
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
		ret = false;
	}
	else
	{
		config = configFile.child("config");
		configApp = config.child("app");
	}

	return ret;
}

// ---------------------------------------------
void App::PrepareUpdate()
{
	frameCount++;
	lastSecFrameCount++;

	dt = frameTime.ReadSec();
	frameTime.Start();
}

// ---------------------------------------------
void App::FinishUpdate()
{
	OPTICK_EVENT("FinishUpdate", Optick::Category::GameLogic);
	if (requestLoad == true)
	{
		Load();
	}

	if (requestSave == true)
	{
		Save();
	}

	if (lastSecFrameTime.Read() > 1000)
	{
		lastSecFrameTime.Start();
		prevLastSecFrameCount = lastSecFrameCount;
		lastSecFrameCount = 0;
	}

	float averageFps = float(frameCount) / startupTime.ReadSec();
	float secondsSinceStartup = startupTime.ReadSec();
	uint32 lastFrameMs = frameTime.Read();
	uint32 framesOnLastUpdate = prevLastSecFrameCount;

	static char title[256];
	sprintf_s(title, 256, "FPS: %d / Av.FPS: %.2f / Last Frame Ms: %02u / Vsync: %s",
		framesOnLastUpdate, averageFps, lastFrameMs, (app->render->vSync)? "on" : "off");

	app->win->SetTitle(title);

	if ((cappedMs > 0.0f) && (lastFrameMs < cappedMs))
	{
		SDL_Delay(cappedMs - lastFrameMs);
	}
}

// Call modules before each loop iteration
bool App::PreUpdate()
{
	OPTICK_EVENT("PreUpdate", Optick::Category::GameLogic);
	bool ret = true;
	ListItem<Module*>* item;
	item = modules.start;
	Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false)
		{
			continue;
		}

		ret = item->data->PreUpdate();
	}

	return ret;
}

// Call modules on each loop iteration
bool App::DoUpdate()
{
	OPTICK_EVENT("DoUpdate", Optick::Category::GameLogic);
	bool ret = true;
	ListItem<Module*>* item;
	item = modules.start;
	Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false)
		{
			continue;
		}

		ret = item->data->Update(dt);
	}

	return ret;
}

// Call modules after each loop iteration
bool App::PostUpdate()
{
	OPTICK_EVENT("PostUpdate", Optick::Category::GameLogic);
	bool ret = true;
	ListItem<Module*>* item;
	Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false)
		{
			continue;
		}

		ret = item->data->PostUpdate();
	}

	return ret;
}

// Called before quitting
bool App::CleanUp()
{
	bool ret = true;
	ListItem<Module*>* item;
	item = modules.end;

	while(item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	return ret;
}

// ---------------------------------------
int App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* App::GetArgv(int index) const
{
	if(index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* App::GetTitle() const
{
	return title.GetString();
}

// ---------------------------------------
const char* App::GetOrganization() const
{
	return organization.GetString();
}

// L02: TODO 5: Create a method to actually load an xml file
// then call all the modules to load themselves

bool App::Load()
{

	bool ret = true;

	pugi::xml_document saveGame;

	pugi::xml_parse_result result = saveGame.load_file(saveFileName);

	if (result == NULL)
	{
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
		ret = false;
	}
	else
	{
		//renderer
		pugi::xml_node rend = saveGame.child("renderer");
		if (rend == NULL)
		{
			LOG("Renderer not loading");
		}

		//input
		pugi::xml_node inp = saveGame.child("input");
		if (inp == NULL)
		{
			LOG("Input not loading");
		}

		//audio
		pugi::xml_node au = saveGame.child("audio");
		if (au == NULL)
		{
			LOG("Audio not loading");
		}

		//scene
		pugi::xml_node sce = saveGame.child("scene");
		if (sce == NULL)
		{
			LOG("Scene not loading");
		}

		//window
		pugi::xml_node wi = saveGame.child("window");
		if (wi == NULL)
		{
			LOG("window not loading");
		}

		pugi::xml_node en = saveGame.child("entities");
		if (en == NULL)
		{
			LOG("entities not loading");
		}

		app->audio->Load(au);
		app->input->Load(inp);
		app->render->Load(rend);
		app->scene->Load(sce);
		app->win->Load(wi);
		app->entities->Load(en);
	}

	requestLoad = false;

	return ret;
}

bool App::Save()
{
	bool ret = true;
	requestSave = false;

	pugi::xml_document newSave;

	pugi::xml_node rend = newSave.append_child("renderer");
	app->render->Save(rend);

	pugi::xml_node audio = newSave.append_child("audio");
	app->audio->Save(audio);

	pugi::xml_node scene = newSave.append_child("scene");
	app->scene->Save(scene);

	pugi::xml_node entities = newSave.append_child("entities");
	app->entities->Save(entities);

	newSave.save_file(saveFileName);

	return ret;
}