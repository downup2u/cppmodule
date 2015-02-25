{
	'includes':[
		'./config.gypi',
	],
	'conditions': [
		['OS=="win"', {
			'variables': {
				'jansson_config_h_dir':'jansson-2.6/win32',
				'mysql_connector_include_dir':'mysql-connector-c-6.1.5-src/include',
			},
		}],
		['OS=="linux"',{ 
			'variables': {
				'jansson_config_h_dir':'jansson-2.6/linux',
				'mysql_connector_include_dir':'mysql-connector-c-6.1.5-src/include',
			},
			'cflags_cc': ['-fexceptions','-frtti','-pthread','-fpermissive','-std=c++11','-fPIC'],
		}]
	]
}