{
	'target_defaults': { 
		'default_configuration': 'Debug', 
		'configurations': { 
			'Debug': { 
				'defines': [ 'DEBUG', '_DEBUG' ], 
				'cflags': [ '-g', '-O0', '-Wall' ], 
				
			}, 
			'Release': { 
				'defines': [ 'NDEBUG' ], 
				'cflags': [ '-O3', '-Wall' ], 				
			}, 
		},
	
	}, 
	'configurations': {
      'Debug': {
        'defines': [
          '_DEBUG'
        ],
        'conditions': [
          ['OS=="linux"', {
            'cflags': ['-g'],
			'cflags_cc': ['-fexceptions','-frtti','-pthread','-fpermissive'],
          }],
          ['OS=="win"', {
		    'library_dirs': [
				'<(module_root_dir)/../libs/windebug',
			],			
            'msvs_settings': {
              'VCCLCompilerTool': {
                'Optimization': '0',
                'MinimalRebuild': 'true',
                'BasicRuntimeChecks': '3',
                'DebugInformationFormat': '4',
                'RuntimeLibrary': '1', 
				'ExceptionHandling':'1',
				'RuntimeTypeInfo':'true',
              },
              'VCLinkerTool': {
                'GenerateDebugInformation': 'true',
                'LinkIncremental': '2'
              }
            }
          }],
          ['OS=="mac"', {
            'xcode_settings': {
              'GCC_GENERATE_DEBUGGING_SYMBOLS': 'YES'
            }
          }]
        ]
      },

      'Release': {
        'conditions': [
          ['OS=="linux"', {
            'cflags': ['-O3'],
			'cflags_cc': ['-fexceptions','-frtti','-pthread','-fpermissive'],			
          }],
		  ['OS=="win"', {
			'msvs_settings': {
              'VCCLCompilerTool': {
                'Optimization': '2',
                'RuntimeLibrary': '0',  
				'ExceptionHandling':'1',
				'RuntimeTypeInfo':'true',				
              }
            }
          }],
          ['OS=="mac"', {
            'xcode_settings': {
              'GCC_GENERATE_DEBUGGING_SYMBOLS': 'NO',
              'GCC_OPTIMIZATION_LEVEL': '3',

              # -fstrict-aliasing. Mainline gcc enables
              # this at -O2 and above, but Apple gcc does
              # not unless it is specified explicitly.
              'GCC_STRICT_ALIASING': 'YES'
            }
          }]
        ]
      }
    },
	'conditions': [
          ['OS=="linux"', {  
			'libraries':[
				'-L../libs/linuxdebug',
				'-lpthread',
				'-lboost_locale',
				'-lboost_log',
				'-lboost_log_setup',
				'-lboost_thread',
				'-lboost_serialization',
				'-lboost_wserialization',
				'-lboost_date_time',
				'-lboost_system',
				'-lboost_chrono',
				'-lboost_filesystem',
				'-lboost_regex',
				'-lsoci_core',
				'-lsoci_mysql',
				'-lmysqlclient',				
			]
          }],
		  ['OS=="win"', {
			'libraries': [
				'../libs/windebug/mysqlclient.lib',
				'../libs/windebug/libsoci_core_3_2.lib',
				'../libs/windebug/libsoci_mysql_3_2.lib',	
	
			],
		 }],
	],
}