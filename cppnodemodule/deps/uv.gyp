{
	'variables': {
		'uv_library':'static_library',		
		'uv_dir':'libuv',		
	},
  'target_defaults': {
    'conditions': [
      ['OS != "win"', {
        'defines': [
          '_LARGEFILE_SOURCE',
          '_FILE_OFFSET_BITS=64',
        ],
        'conditions': [
          ['OS=="solaris"', {
            'cflags': [ '-pthreads' ],
          }],
          ['OS not in "solaris android"', {
            'cflags': [ '-pthread' ],
          }],
        ],
      }],
    ],
    'xcode_settings': {
      'WARNING_CFLAGS': [ '-Wall', '-Wextra', '-Wno-unused-parameter' ],
      'OTHER_CFLAGS': [ '-g', '--std=gnu89', '-pedantic' ],
    }
  },

  'targets': [
    {
      'target_name': 'libuv',
      'type': '<(uv_library)',
	  'includes':[
		'./config.gypi',
	  ],
      'include_dirs': [
        '<(uv_dir)/include',
        '<(uv_dir)/src/',
      ],
      'direct_dependent_settings': {
        'include_dirs': [ 'include' ],
        'conditions': [
          ['OS != "win"', {
            'defines': [
              '_LARGEFILE_SOURCE',
              '_FILE_OFFSET_BITS=64',
            ],
          }],
          ['OS == "mac"', {
            'defines': [ '_DARWIN_USE_64_BIT_INODE=1' ],
          }],
          ['OS == "linux"', {
            'defines': [ '_POSIX_C_SOURCE=200112' ],
          }],
        ],
      },
      'sources': [
        '<(uv_dir)/common.gypi',
        '<(uv_dir)/include/uv.h',
        '<(uv_dir)/include/tree.h',
        '<(uv_dir)/include/uv-errno.h',
        '<(uv_dir)/include/uv-threadpool.h',
        '<(uv_dir)/include/uv-version.h',
        '<(uv_dir)/src/fs-poll.c',
        '<(uv_dir)/src/heap-inl.h',
        '<(uv_dir)/src/inet.c',
        '<(uv_dir)/src/queue.h',
        '<(uv_dir)/src/threadpool.c',
        '<(uv_dir)/src/uv-common.c',
        '<(uv_dir)/src/uv-common.h',
        '<(uv_dir)/src/version.c'
      ],
      'conditions': [
        [ 'OS=="win"', {
          'defines': [
            '_WIN32_WINNT=0x0600',
            '_GNU_SOURCE',
          ],
          'sources': [
            '<(uv_dir)/include/uv-win.h',
            '<(uv_dir)/src/win/async.c',
            '<(uv_dir)/src/win/atomicops-inl.h',
            '<(uv_dir)/src/win/core.c',
            '<(uv_dir)/src/win/dl.c',
            '<(uv_dir)/src/win/error.c',
            '<(uv_dir)/src/win/fs.c',
            '<(uv_dir)/src/win/fs-event.c',
            '<(uv_dir)/src/win/getaddrinfo.c',
            '<(uv_dir)/src/win/getnameinfo.c',
            '<(uv_dir)/src/win/handle.c',
            '<(uv_dir)/src/win/handle-inl.h',
            '<(uv_dir)/src/win/internal.h',
            '<(uv_dir)/src/win/loop-watcher.c',
            '<(uv_dir)/src/win/pipe.c',
            '<(uv_dir)/src/win/thread.c',
            '<(uv_dir)/src/win/poll.c',
            '<(uv_dir)/src/win/process.c',
            '<(uv_dir)/src/win/process-stdio.c',
            '<(uv_dir)/src/win/req.c',
            '<(uv_dir)/src/win/req-inl.h',
            '<(uv_dir)/src/win/signal.c',
            '<(uv_dir)/src/win/stream.c',
            '<(uv_dir)/src/win/stream-inl.h',
            '<(uv_dir)/src/win/tcp.c',
            '<(uv_dir)/src/win/tty.c',
            '<(uv_dir)/src/win/timer.c',
            '<(uv_dir)/src/win/udp.c',
            '<(uv_dir)/src/win/util.c',
            '<(uv_dir)/src/win/winapi.c',
            '<(uv_dir)/src/win/winapi.h',
            '<(uv_dir)/src/win/winsock.c',
            '<(uv_dir)/src/win/winsock.h',
          ],
          'link_settings': {
            'libraries': [
              '-ladvapi32',
              '-liphlpapi',
              '-lpsapi',
              '-lshell32',
              '-lws2_32'
            ],
          },
        }, { # Not Windows i.e. POSIX
          'cflags': [
            '-g',
            '--std=gnu89',
            '-pedantic',
            '-Wall',
            '-Wextra',
            '-Wno-unused-parameter',
          ],
          'sources': [
            '<(uv_dir)/include/uv-unix.h',
            '<(uv_dir)/include/uv-linux.h',
            '<(uv_dir)/include/uv-sunos.h',
            '<(uv_dir)/include/uv-darwin.h',
            '<(uv_dir)/include/uv-bsd.h',
            '<(uv_dir)/include/uv-aix.h',
            '<(uv_dir)/src/unix/async.c',
            '<(uv_dir)/src/unix/atomic-ops.h',
            '<(uv_dir)/src/unix/core.c',
            '<(uv_dir)/src/unix/dl.c',
            '<(uv_dir)/src/unix/fs.c',
            '<(uv_dir)/src/unix/getaddrinfo.c',
            '<(uv_dir)/src/unix/getnameinfo.c',
            '<(uv_dir)/src/unix/internal.h',
            '<(uv_dir)/src/unix/loop.c',
            '<(uv_dir)/src/unix/loop-watcher.c',
            '<(uv_dir)/src/unix/pipe.c',
            '<(uv_dir)/src/unix/poll.c',
            '<(uv_dir)/src/unix/process.c',
            '<(uv_dir)/src/unix/signal.c',
            '<(uv_dir)/src/unix/spinlock.h',
            '<(uv_dir)/src/unix/stream.c',
            '<(uv_dir)/src/unix/tcp.c',
            '<(uv_dir)/src/unix/thread.c',
            '<(uv_dir)/src/unix/timer.c',
            '<(uv_dir)/src/unix/tty.c',
            '<(uv_dir)/src/unix/udp.c',
          ],
          'link_settings': {
            'libraries': [ '-lm' ],
            'conditions': [
              ['OS=="solaris"', {
                'ldflags': [ '-pthreads' ],
              }],
              ['OS != "solaris" and OS != "android"', {
                'ldflags': [ '-pthread' ],
              }],
            ],
          },
          'conditions': [
            ['uv_library=="shared_library"', {
              'cflags': [ '-fPIC' ],
            }],
            ['uv_library=="shared_library" and OS!="mac"', {
              'link_settings': {
                # Must correspond with UV_VERSION_MAJOR and UV_VERSION_MINOR
                # in include/uv-version.h
                'libraries': [ '-Wl,-soname,libuv.so.1.0' ],
              },
            }],
          ],
        }],
        [ 'OS in "linux mac android"', {
          'sources': [ '<(uv_dir)/src/unix/proctitle.c' ],
        }],
        [ 'OS=="mac"', {
          'sources': [
            '<(uv_dir)/src/unix/darwin.c',
            '<(uv_dir)/src/unix/fsevents.c',
            '<(uv_dir)/src/unix/darwin-proctitle.c',
          ],
          'defines': [
            '_DARWIN_USE_64_BIT_INODE=1',
            '_DARWIN_UNLIMITED_SELECT=1',
          ]
        }],
        [ 'OS!="mac"', {
          # Enable on all platforms except OS X. The antique gcc/clang that
          # ships with Xcode emits waaaay too many false positives.
          'cflags': [ '-Wstrict-aliasing' ],
        }],
        [ 'OS=="linux"', {
          'defines': [ '_GNU_SOURCE' ],
          'sources': [
            '<(uv_dir)/src/unix/linux-core.c',
            '<(uv_dir)/src/unix/linux-inotify.c',
            '<(uv_dir)/src/unix/linux-syscalls.c',
            '<(uv_dir)/src/unix/linux-syscalls.h',
          ],
          'link_settings': {
            'libraries': [ '-ldl', '-lrt' ],
          },
        }],
        [ 'OS=="android"', {
          'sources': [
            '<(uv_dir)/src/unix/linux-core.c',
            '<(uv_dir)/src/unix/linux-inotify.c',
            '<(uv_dir)/src/unix/linux-syscalls.c',
            '<(uv_dir)/src/unix/linux-syscalls.h',
            '<(uv_dir)/src/unix/pthread-fixes.c',
            '<(uv_dir)/src/unix/android-ifaddrs.c'
          ],
          'link_settings': {
            'libraries': [ '-ldl' ],
          },
        }],
        [ 'OS=="solaris"', {
          'sources': [ 'src/unix/sunos.c' ],
          'defines': [
            '__EXTENSIONS__',
            '_XOPEN_SOURCE=500',
          ],
          'link_settings': {
            'libraries': [
              '-lkstat',
              '-lnsl',
              '-lsendfile',
              '-lsocket',
            ],
          },
        }],
        [ 'OS=="aix"', {
          'sources': [ '<(uv_dir)/src/unix/aix.c' ],
          'defines': [
            '_ALL_SOURCE',
            '_XOPEN_SOURCE=500',
            '_LINUX_SOURCE_COMPAT',
          ],
          'link_settings': {
            'libraries': [
              '-lperfstat',
            ],
          },
        }],
        [ 'OS=="freebsd" or OS=="dragonflybsd"', {
          'sources': [ '<(uv_dir)/src/unix/freebsd.c' ],
        }],
        [ 'OS=="openbsd"', {
          'sources': [ '<(uv_dir)/src/unix/openbsd.c' ],
        }],
        [ 'OS=="netbsd"', {
          'sources': [ '<(uv_dir)/src/unix/netbsd.c' ],
        }],
        [ 'OS in "freebsd dragonflybsd openbsd netbsd".split()', {
          'link_settings': {
            'libraries': [ '-lkvm' ],
          },
        }],
        [ 'OS in "mac freebsd dragonflybsd openbsd netbsd".split()', {
          'sources': [ '<(uv_dir)/src/unix/kqueue.c' ],
        }],
        ['uv_library=="shared_library"', {
          'defines': [ 'BUILDING_UV_SHARED=1' ]
        }],
      ]
    },   
    
  ]
}
