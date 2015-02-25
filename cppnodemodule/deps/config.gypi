{
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
			'cflags_cc': ['-fexceptions','-frtti','-pthread','-fpermissive','-fPIC'],
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
}