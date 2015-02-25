{

   'targets': [
    {  
		'target_name': "testsocisample",
		'type':'executable',	
		'include_dirs': [
			'../../deps/boost_1_57_0',
			'../../deps/soci-3.2.2/core',
			'../../deps/soci-3.2.2/backends/mysql',
			'../../deps/mysql-connector-c-6.1.5-src/include',
		],
		'sources': [ 
			'../src/sample_soci_mysql.cc',		
		],
		'conditions': [
			['OS=="win"', {
				'library_dirs': [
					'<../../libs/win',
				],
				'libraries': [
					'../../libs/win/mysqlclient.lib',
					'../../libs/win/libsoci_core_3_2.lib',
					'../../libs/win/libsoci_mysql_3_2.lib',	
					'Advapi32.lib',
				],
				'msvs_settings': {
					'VCCLCompilerTool': {
					'Optimization': '2',
					'RuntimeLibrary': '0',  # /MT==>'2':/MD   
					'ExceptionHandling':'1',
					'RuntimeTypeInfo':'true',				
              }
            }
			}],
			['OS=="linux"',
			{ 
				'cflags_cc': ['-fexceptions','-frtti','-pthread','-fpermissive'],
				'link_settings':{
					'libraries':[
					'-lpthread',
					'-lrt',
					'-ldl',
					],
				 },
				'libraries':[
					'-L../../libs/linux',
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
			}]
		]
    }]   
  
}
