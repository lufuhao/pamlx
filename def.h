//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#ifndef DEF_H
#define DEF_H

#ifndef MAX_PATH

#ifdef _MAX_PATH
#define MAX_PATH _MAX_PATH

#else
#define MAX_PATH FILENAME_MAX

#endif // _MAX_PATH

#endif // MAX_PATH

#endif // DEF_H
