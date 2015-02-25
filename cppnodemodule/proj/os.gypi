{
	'includes':[
		'./config.gypi',
	],
	'conditions': [
		['OS=="win"', {
			'library_dirs': [
				'<(module_root_dir)/../libs/win',
			],
			'libraries': [
				'<(module_root_dir)/../libs/win/mysqlclient.lib',
				'<(module_root_dir)/../libs/win/libsoci_core_3_2.lib',
				'<(module_root_dir)/../libs/win/libsoci_mysql_3_2.lib',		
			],
		}],
		['OS=="linux"',
		{ 
			'cflags_cc': ['-fexceptions','-frtti','-pthread','-fpermissive'],
			'libraries':[
				'-L<(module_root_dir)/../libs/linux',
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
		}]
	]
}
