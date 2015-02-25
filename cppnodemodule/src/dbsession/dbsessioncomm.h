#ifndef _DBSESSIONCOMM_INCLUDE
#define _DBSESSIONCOMM_INCLUDE
#include <string>
#include <map>
#include "../mylib/util/utiltime.h"
#include "../mylib/db/mysqldbutil.h"
#include "../gcomm/comm.pb.h"
#include "../gcomm/session.pb.h"
#include <encryptstring.h>

template<> struct soci::type_conversion<comm::PkgTaskInfo>
{
	typedef soci::values base_type;

	static void from_base(soci::values const &v, soci::indicator /* ind */, comm::PkgTaskInfo &t)
	{
		// here we ignore the possibility the the whole object might be NULL
		t.set_id(v.get<int>("id"));
		t.set_parentid(v.get<int>("partentid"));
		std::string task_name_decoded;
		decodeString_Hex(v.get<std::string>("name", ""), task_name_decoded);
		t.set_name(task_name_decoded);
		t.set_taskdate(v.get<std::string>("taskdate", ""));
		std::string task_content_decoded;
		decodeString_Hex(v.get<std::string>("content", ""), task_content_decoded);
		t.set_content(task_content_decoded);
		std::string task_remark_decoded;
		decodeString_Hex(v.get<std::string>("remark", ""), task_remark_decoded);
		t.set_remark(task_remark_decoded);
		int nLevel = v.get<int>("tasklevel");
		t.set_tasklevel((comm::PkgTaskInfo::EnTaskLevel)nLevel);
		t.set_createuserid(v.get<int>("createuserid"));
		int nStatus = v.get<int>("status");
		t.set_status((comm::PkgTaskInfo::EnTaskStatus)nStatus);

		tm t1 = { 0 };
		t1 = v.get<tm>("createtime", t1);
		t.set_createtime(UtilTime::convertToTimeString(t1));
		tm t2 = { 0 };
		t2 = v.get<tm>("finishtime", t2);
		t.set_finishtime(UtilTime::convertToTimeString(t2));
		t.set_sortflag(v.get<int>("sortflag",0));
		t.set_accepteduserid(v.get<int>("accepteduserid"));
		t.set_coworkid(v.get<int>("coworkid"));
		tm t3 = { 0 };
		t3 = v.get<tm>("updatetime", t3);
		t.set_updatetime(UtilTime::convertToTimeString(t3));
		t.set_coworkid(v.get<int>("coworkid"));
		t.set_isreaded(v.get<int>("isreaded"));

	}

	/*static void to_base(comm::PkgTaskInfo const &t, soci::values &v, soci::indicator &ind)
	{
		v.set("id", t.id());
		v.set("partentid", t.parentid());
		v.set("taskdate", t.taskdate());
		v.set("name", t.name());
		v.set("content", t.content());
		v.set("tasklevel", (int)t.tasklevel());
		v.set("remark", t.remark());
		v.set("createuserid", t.createuserid());
		v.set("status", (int)t.status());
		tm tm1 = { 0 };
		tm tm2 = { 0 };
		tm tm3 = { 0 };
		UtilTime::convertStringToTime(t.createtime(), tm1);
		v.set("createtime", tm1);
		UtilTime::convertStringToTime(t.finishtime(), tm2);
		v.set("finishtime", tm2);
		UtilTime::convertStringToTime(t.updatetime(), tm3);
		v.set("updatetime", tm3);
		v.set("sortflag", t.sortflag());
		v.set("accepteduserid", t.accepteduserid());
		v.set("coworkid", t.coworkid());
		v.set("isreaded", t.isreaded());
		ind = i_ok;
	}*/
};



template<> struct soci::type_conversion<comm::PkgAdvInfo>
{
	typedef soci::values base_type;

	static void from_base(soci::values const &v, soci::indicator /* ind */, comm::PkgAdvInfo &t)
	{
		// here we ignore the possibility the the whole object might be NULL
		t.set_id(v.get<int>("id"));
		std::string advtext_decoded;
		std::string advdesc_decoded;
		decodeString_Hex(v.get<std::string>("advtxt", ""), advtext_decoded);
		decodeString_Hex(v.get<std::string>("advdesc", ""), advdesc_decoded);
		t.set_name(v.get<std::string>("name", ""));
		t.set_advtxt(advtext_decoded);
		t.set_advdesc(advdesc_decoded);
		tm t1 = { 0 };
		t1 = v.get<tm>("createtime", t1);
		t.set_createtime(UtilTime::convertToTimeString(t1));
		tm t2 = { 0 };
		t2 = v.get<tm>("expiretime", t2);
		t.set_expiretime(UtilTime::convertToTimeString(t2));
	}

	static void to_base(comm::PkgAdvInfo const &t, soci::values &v, soci::indicator &ind)
	{
		v.set("id", t.id());
		v.set("name", t.name());
		v.set("advtxt", t.advtxt());
		v.set("advdesc", t.advdesc());
		tm tm1 = { 0 };
		tm tm2 = { 0 };
		UtilTime::convertStringToTime(t.createtime(), tm1);
		v.set("createtime", tm1);
		UtilTime::convertStringToTime(t.expiretime(), tm2);
		v.set("expiretime", tm2);
		ind = i_ok;
	}
};

template<> struct soci::type_conversion<::session::PkgRoleUser>
{
	typedef soci::values base_type;

	static void from_base(soci::values const &v, soci::indicator /* ind */, ::session::PkgRoleUser &t)
	{
		// here we ignore the possibility the the whole object might be NULL
		t.set_id(v.get<int>("id"));
		t.set_rolename(v.get<std::string>("rolename", ""));
		t.set_roledesc(v.get<std::string>("roledesc", ""));
		t.set_permission(v.get<int>("permission"));
	}

	static void to_base(::session::PkgRoleUser const &t, soci::values &v, soci::indicator &ind)
	{
		v.set("id", t.id());
		v.set("rolename", t.rolename());
		v.set("roledesc", t.roledesc());
		v.set("permission", t.permission());
		ind = i_ok;
	}
};

	template<> struct soci::type_conversion<::session::PkgUser>
	{
		typedef soci::values base_type;

		static void from_base(soci::values const &v, soci::indicator /* ind */, ::session::PkgUser &t)
		{
			std::string user_password_decoded;
			std::string user_realname_decoded;
			std::string user_phonenumber_decoded;
			std::string user_emailaddress_decoded;
			// here we ignore the possibility the the whole object might be NULL
			t.set_userid(v.get<int>("id"));
			t.set_username(v.get<std::string>("username", ""));
			decodeString(v.get<std::string>("password", ""),user_password_decoded);
			t.set_password(user_password_decoded);

			decodeString(v.get<std::string>("realname", ""), user_realname_decoded);
			t.set_realname(user_realname_decoded);
			decodeString(v.get<std::string>("phonenumber", ""), user_phonenumber_decoded);
			t.set_phonenumber(user_phonenumber_decoded);

			decodeString(v.get<std::string>("emailaddress", ""), user_emailaddress_decoded);
			t.set_emailaddress(user_emailaddress_decoded);

			t.set_permission(v.get<int>("permission"));
			t.set_companyid(v.get<int>("companyid"));
			tm t1 = { 0 };
			t1 = v.get<tm>("createtime", t1);
			t.set_createtime(UtilTime::convertToTimeString(t1));
			t.set_permissionroleid(v.get<int>("permissionroleid"));

		}

		/*static void to_base(::session::PkgUser const &t, soci::values &v, soci::indicator &ind)
		{
			v.set("id", t.userid());
			v.set("username", t.username());
			v.set("password", t.password());
			v.set("realname", t.realname());
			v.set("phonenumber", t.phonenumber());
			v.set("emailaddress", t.emailaddress());
			v.set("permission", t.permission());
			v.set("companyid", t.companyid());
			tm tm1 = { 0 };
			UtilTime::convertStringToTime(t.createtime(), tm1);
			v.set("createtime", tm1);
			v.set("permissionroleid", t.permissionroleid());
			ind = i_ok;
		}*/
	};


struct userInfo{
	std::string sLoginSn;
	std::string sLastLoginTime;
	int notifyupdatedflag;
	std::string sUpdatedTime;
};

class DBSessionComm
{
public:
	DBSessionComm();
	~DBSessionComm();
private:
	mysqlDBUtil dbUtil_;
public:
	bool initSession(size_t nPoolSize, const std::string& connectstring);
	bool exitSession(std::string&err);
public:
	bool handleMessage_userLogin(boost::shared_ptr<comm::PkgUserLoginReq> pPkgUserLoginReq, boost::shared_ptr<comm::PkgUserLoginReply> pPkgUserLoginReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool handleMessage_userCheck(boost::shared_ptr<comm::PkgUserCheckReq> pPkgUserCheckReq, boost::shared_ptr<comm::PkgUserCheckReply> pPkgUserCheckReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool handleMessage_userGetAuth(boost::shared_ptr<comm::PkgUserGetAuthReq> pPkgUserGetAuthReq, boost::shared_ptr<comm::PkgUserGetAuthReply> pPkgUserGetAuthReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool handleMessage_userCreateUser(boost::shared_ptr<comm::PkgUserCreateReq> pPkgUserCreateReq, boost::shared_ptr<comm::PkgUserCreateReply> pPkgUserCreateReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool handleMessage_userGetPassword(boost::shared_ptr<comm::PkgUserGetPasswordReq> pPkgUserGetPasswordReq, boost::shared_ptr<comm::PkgUserGetPasswordReply> pPkgUserGetPasswordReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser, ::session::EnAuthType type = ::session::EA_FINDPASSWORD);
	bool handleMessage_userResetPassword(boost::shared_ptr<comm::PkgUserResetPasswordReq> pPkgUserResetPasswordReq, boost::shared_ptr<comm::PkgUserResetPasswordReply> pPkgUserResetPasswordReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool handleMessage_userSet(boost::shared_ptr<comm::PkgUserSetReq> pPkgUserSetReq, boost::shared_ptr<comm::PkgUserSetReply> pPkgUserSetReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool handleMessage_userInvitionUser(boost::shared_ptr<comm::PkgUserInvitionUserReq> pPkgUserInvitionUserReq, boost::shared_ptr<comm::PkgUserInvitionUserReply> pPkgUserInvitionUserReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool handleMessage_userCheckInvitionCode(boost::shared_ptr<comm::PkgUserCheckInvitionCodeReq> pPkgUserCheckInvitionCodeReq, boost::shared_ptr<comm::PkgUserCheckInvitionCodeReply> pPkgUserCheckInvitionCodeReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool handleMessage_userInvitionCheck(boost::shared_ptr<comm::PkgUserInvitionCheckReq> pPkgUserInvitionCheckReq, boost::shared_ptr<comm::PkgUserInvitionCheckReply> pPkgUserInvitionCheckReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool handleMessage_userCreateCompany(boost::shared_ptr<comm::PkgUserCreateCompanyReq> pPkgUserCreateCompanyReq, boost::shared_ptr<comm::PkgUserCreateCompanyReply> pPkgUserCreateCompanyReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool handleMessage_userReassignAdmin(boost::shared_ptr<comm::PkgUserReassignAdminReq> pPkgUserReassignAdminReq, boost::shared_ptr<comm::PkgUserReassignAdminReply> pPkgUserReassignAdminReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool handleMessage_userDeleteFromCompany(boost::shared_ptr<comm::PkgUserDeleteFromCompanyReq> pPkgUserDeleteFromCompanyReq, boost::shared_ptr<comm::PkgUserDeleteFromCompanyReply> pPkgUserDeleteFromCompanyReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool handleMessage_userExitFromCompany(boost::shared_ptr<comm::PkgUserExitCompanyReq> pPkgUserExitCompanyReq, boost::shared_ptr<comm::PkgUserExitCompanyReply> pPkgUserExitCompanyReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);

	bool handleMessage_taskQuery(boost::shared_ptr<comm::PkgTaskQueryReq> pPkgTaskQueryReq, boost::shared_ptr<comm::PkgTaskQueryReply> pPkgTaskQueryReply,boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool handleMessage_taskPageQuery(boost::shared_ptr<comm::PkgTaskPageQueryReq> pPkgTaskPageQueryReq, boost::shared_ptr<comm::PkgTaskPageQueryReply> pPkgTaskPageQueryReply,boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool handleMessage_taskSetSortFlag(boost::shared_ptr<comm::PkgTaskSetSortFlagReq> pPkgTaskSetOrderReq, boost::shared_ptr<comm::PkgTaskSetSortFlagReply> pPkgTaskSetOrderReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool handleMessage_taskOperation(boost::shared_ptr<comm::PkgTaskOperationReq> pPkgTaskOperationReq, boost::shared_ptr<comm::PkgTaskOperationReply> pPkgTaskOperationReply,boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	
	bool handleMessage_groupQuery(boost::shared_ptr<comm::PkgCompanyGroupQueryReq> pPkgCompanyGroupQueryReq, boost::shared_ptr<comm::PkgCompanyGroupQueryReply> pPkgCompanyGroupQueryReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);

	bool handleMessage_advQuery(boost::shared_ptr<comm::PkgAdvQueryReq> pPkgAdvQueryReq, boost::shared_ptr<comm::PkgAdvQueryReply> pPkgAdvQueryReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	
	bool handleMessage_sysQueryMyProfile(boost::shared_ptr<comm::PkgQueryMyProfileReq> pPkgQueryMyProfileReq, boost::shared_ptr<comm::PkgQueryMyProfileReply> pPkgQueryMyProfileReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
private:
	std::string generateAuthCode();
	std::map<int, boost::shared_ptr<::session::PkgRoleUser>> mapRoles;
private:
	std::string getTaskPageQueryWhereCondition(int nUserid, boost::shared_ptr<comm::PkgTaskPageQueryReq> pPkgTaskPageQueryReq);
	void onUpdateTask(const comm::PkgTaskInfo&newtask, const comm::PkgTaskInfo&oldtask, int nUserid, soci::session& sql);
	void onSortCurTask(int taskid, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser, soci::session& sql);
	void sortDateTask(const std::string&sTaskDate, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	void sortTodayTaskArray(std::vector<boost::shared_ptr<comm::PkgTaskInfo>>& vectTasks, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool isUserAdmin(boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	std::string getUserCompanyId(boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	void getSysinfoMap(std::map<std::string, std::string>&mapSysInfo, soci::session& sql);
	void getCompanyUsers(int companyid, std::vector<::session::PkgUser>&vectUsers, soci::session& sql);
	void getCoworkUserIds(int coworktaskid, int&createuserid, std::vector<int>&vectAcceptsIds, soci::session& sql);
	void getUserMap(std::map<int, std::string>& mapUseRealname, soci::session& sql);
	bool isMyTaskNotFinished(boost::shared_ptr<comm::PkgTaskInfo> p, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool isTaskDisableEdit(const comm::PkgTaskInfo&pkgTaskInfo, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool isTaskDisableUpdateStatus(const comm::PkgTaskInfo&pkgTaskInfo,int updatestatus, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool getTaskDateBeforeUpdate(const comm::PkgTaskInfo&pkgTaskInfo, std::string&oldDate, std::string&newDate, soci::session& sql);
	bool getTaskInfo(comm::PkgTaskInfo& pkgTaskInfo, int taskid, soci::session& sql);
	bool updateCoworkTaskReaded(comm::PkgTaskInfo& pkgTaskInfo, int isreaded, int nUserid, soci::session& sql);
public:
	comm::PkgCompanyInfo*getCompanyInfo(boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);

	comm::PkgCompanyInfo*getCompanyInfo(boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser, soci::session& sql);
	comm::PkgCompanyInfo*getCompanyInfo(int companyid, soci::session& sql);
	void getTaskNumber(boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser, int&taskFinished, int&taskNotFinished);
	void getLastestVersion(const std::string&platform, std::string &versionlastest, std::string &versiondownloadurl);
	boost::shared_ptr<::session::PkgRoleUser> getRoles(boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);

public:
	//FOR NOTIFY
	void setUserLoginSn(int userid, const std::string&sLoginSn);
	void isSessionUserVaildWithException(boost::shared_ptr<session::PkgSessionUser>pPkgSessionUser);//直接抛出异常
	int getUserUpdateFlag(int userid, std::string&sUpdatedTime);
	void updateUserUpdateFlags(std::vector<int>&vectUsers, int nUFlag, soci::session& sql);
	void updateUserUpdateFlags(int userid, int nUFlag, soci::session& sql);
	void clearUserUpdateFlag(int userid, int nUFlag);

	bool getUserNotify(int userid, userInfo&user, soci::session& sql);
	void setUserNotify(int userid, userInfo&user, bool bupdate /*= true,*/, soci::session& sql);
};

#endif