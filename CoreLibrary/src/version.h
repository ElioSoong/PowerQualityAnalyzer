#ifndef _AUTO_VERSION_H
#define _AUTO_VERSION_H

/*********************************************************************************************************
** Auto-increment version [ DO NOT EDIT THE REVISION_VERSION & BUILD_COUNT MACRO]
** 
** REVISION_VERSION: Default as 4715, which indicates SVN revision id.
** BUILD_COUNT:      Default as 0, which indicates the counter for build.
**
** EDIT THE TEMPLATE FILE FOR MAJOR & MINOR VERSION INCREAMENT.
*********************************************************************************************************/
#define MAJOR_VERSION                       0
#define MINOR_VERSION                       031

#define REVISION_VERSION                    4513
#define BUILD_COUNT                         942

#define __VSTR__(x)                         #x
#define __VSTR(x)                           __VSTR__(x)
#define VERSION_STRING                      __VSTR(MAJOR_VERSION)"."                                    \
                                            __VSTR(MINOR_VERSION)"."                                    \
                                            __VSTR(REVISION_VERSION)"."                                 \
                                            __VSTR(BUILD_COUNT)


#endif                                                                  /* #ifndef _AUTO_VERSION_H      */
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
