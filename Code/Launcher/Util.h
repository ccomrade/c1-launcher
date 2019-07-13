/**
 * @file
 * @brief Utilities.
 */

#pragma once

int FillNOP( void *address, size_t length );
int FillMem( void *address, void *data, size_t length );

int GetCrysisGameVersion( void *lib );

bool HasAMDProcessor();
bool Is3DNowSupported();

