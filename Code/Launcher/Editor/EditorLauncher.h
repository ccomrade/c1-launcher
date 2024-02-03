#pragma once

class EditorLauncher
{
	struct DLLs
	{
		void* pEditor;
		void* pCrySystem;
		void* pCryRenderD3D9;
		void* pCryRenderD3D10;

		int editorBuild;
		int gameBuild;
	};

	DLLs m_dlls;

public:
	EditorLauncher();
	~EditorLauncher();

	int Run(char* cmdLine);

private:
	void LoadEngine();
	void PatchEngine();
};
