#pragma once

struct FileBase
{
	enum Access
	{
		READ_ONLY,
		WRITE_ONLY,
		WRITE_ONLY_CREATE,
		READ_WRITE,
		READ_WRITE_CREATE,
	};

	enum SeekBase
	{
		BEGIN,
		CURRENT,
		END,
	};
};
