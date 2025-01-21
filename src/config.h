#pragma once

#define ERR_BUF_LEN     512             // maximum allowed length of error message
#define READ_BUF_LEN    64              // initial read buffer length (dynamically doubles when exhausted)
#define REPL_IN         "8=> "
#define LANG_NAME       "PickleLisp"
#define EXTENSION       ".pickle"       // pickle scripts extension

#define PUBLIC          // Nothing -> just using it for the visual queue that something
                        // will be used outside of the translation unit