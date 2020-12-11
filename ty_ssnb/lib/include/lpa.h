#ifndef _LPA_H_
#define _LPA_H_

//#define EC20_HTTP
//#define EC20_UDP
//#define N58_PLAT
#define WINDOWS

#ifdef __cplusplus
extern "C"{
#endif


typedef unsigned char UCHAR;
typedef unsigned long ULONG;
typedef unsigned int UINT;

#define profileMaxNum 6

	typedef struct T_Lpa_Ctx
	{
		#ifdef ANDROID_JNI_PLAT
		JNIEnv* env;
		#else
		void* handle;
		#endif
	}Lpa_Ctx;

	typedef struct
	{
		char* iccid;
		char* profileNickName;
		char* profileState;
		char* serviceproviderName;
		char* profileName;
		char* iconType;
		char* icon;
		char* profileClass;
		char* notificationConfigurationInfo;
		char* profileOwner;
		char* dpProprietaryData;
		char* profilePolicyRules;
		char* isdPAid;
		char* operaMark;
		char* writeState;
		char* phoneNumber;
		char* packageName;
	}ProfileInfo;

	typedef struct
	{
		int profileSize;
		ProfileInfo profileEntity[profileMaxNum];
	}ProfileInfoList;

	typedef struct
	{
		char* pprId;
		char* allowedOperator;
		char* endUserConsent;
	}RATBean;

	typedef struct
	{
		int size;
		RATBean* entity;
	}RATBeanList;


	typedef enum
	{
		LpaOk = 0,
		OpenchannelFailed = -1,
		SelecteSIMFailed = -2,
		GetEuiccChallengeFailed = -3,
		GetEuiccInfoFailed = -4,
		ServerUnreach = -5,
		InstallProfileFailed = -6,
		GetProfileInfoFailed = -7,
		ProfileInfoError = -8,
		GetEIDFailed = -9,
		EIDInfoFailed = -10,
		DELETEFailed = -11,
		EnableFailed = -12,
		DisableFailed = -13,
		ParaError = -14,
		MemeryError = -15,
		ApduTransmitError = -16,
		unknow = -17,
		SendApduFaild = -18,
		CardError_6D00 = -19,
		SetNickNameFailed = -20,
		SetDefaultDpAddressFailed = -21,
		GetEuiccConfiguredAddressFailed = -22,
		eUICCMemoryResetFailed = -23,
		RetrieveNotifyListFailed = -24,
		ConfirmCodeError = -25,
		PPRnotAllowed = -26,
		NetworkApiUnset = -27,
		CmdApiUnset = -28,
	}lpa_result_t;

	typedef struct
	{
		lpa_result_t errcode;
		char *description;
	}T_LpaErrorReason;

	typedef enum {
		enable_iccidOrAidNotFound = 0x01,
		enable_profileNotInDisabledState = 0x02,
		enable_disallowedByPolicy = 0x03,
		enable_wrongProfileReenabling = 0x4,
		enable_catBusy = 0x05,
		enable_undefinedError = 0x7F,
	}lpa_enable_result_t;

	typedef enum {
		disable_iccidOrAidNotFound = 0x01,
		disable_profileNotInEnabledState = 0x02,
		disable_disallowedByPolicy = 0x03,
		disable_catBusy = 0x05,
		disable_undefinedError = 0x7F,
	}lpa_disable_result_t;


	typedef enum {
		delete_iccidOrAidNotFound = 0x01,
		delete_profileNotInDisabledState = 0x02,
		delete_disallowedByPolicy = 0x03,
		delete_undefinedError = 0x7F,
	}lpa_delete_result_t;


	typedef enum {
		ERR_ICCIDORAID = 0x01,
		ERR_PRFILE_STATE = 0x02,
		ERR_PPRNOTALLOWED = 0x03,
		ERR_SYSTEMBUSY = 0x05,
		ERR_UNDEFINED = 0x7F,
	}LOCAL_ERR_TYPE;

	typedef enum
	{
		LPA_ERR_OK = (0),
		ERR_incorrectInputValues = (1),
		ERR_invalidSignature = (2),
		ERR_invalidTransactionId = (3),
		ERR_unsupportedCrtValues = (4),
		ERR_unsupportedRemoteOperationType = (5),
		ERR_unsupportedProfileClass = (6),
		ERR_scp03tStructureError = (7),
		ERR_scp03tSecurityError = (8),
		ERR_installFailedDueToIccidAlreadyExistsOnEuicc = (9),
		ERR_installFailedDueToInsufficientMemoryForProfile = (10),
		ERR_installFailedDueToInterruption = (11),
		ERR_installFailedDueToPEProcessingError = (12),
		ERR_installFailedDueToDataMismatch = (13),
		ERR_testProfileInstallFailedDueToInvalidNaaKey = (14),
		ERR_pprNotAllowed = (15),
		ERR_installFailedDueToUnknownError = (127),
	}InstallErrorCode;


	typedef enum
	{
		ERR_auth_invalidCertificate = (1),
		ERR_auth_invalidSignature = (2),
		ERR_auth_unsupportedCurve = (3),
		ERR_auth_noSessionContext = (4),
		ERR_auth_invalidOid = (5),
		ERR_auth_euiccChallengeMismatch = (6),
		ERR_auth_ciPKUnknown = (7),
		ERR_auth_undefinedError = (127),
	}AuthenticateErrorCode;


	typedef enum
	{
		ERR_dld_invalidCertificate = (1),
		ERR_dld_invalidSignature = (2),
		ERR_dld_unsupportedCurve = (3),
		ERR_dld_noSessionContext = (4),
		ERR_dld_invalidTransactionId = (5),
		ERR_dld_undefinedError = (127),
	}DownloadErrorCode;

	typedef struct
	{
		InstallErrorCode errcode;
		char *description;
	}T_ErrorReason;


	typedef struct
	{
		AuthenticateErrorCode errcode;
		char *description;
	}T_ErrorAuthReason;


	typedef struct
	{
		DownloadErrorCode errcode;
		char *description;
	}T_ErrorDldReason;

	typedef struct
	{
		char* iccid;
		char* serviceProviderName;
		char* profileName;
		char* iconType;
		char* icon;
		char* profileClass;
		char* notificationConfigurationInfo;
		char* profileOwner;
		char* profilePolicyRules;
	}ProfileMetadata;

	typedef struct
	{
		char* notification;
		char* seqNumber;
		char* profileManagementOperation;
		char* notificationAddress;
		char* iccid;
	}PendingNotification;

	typedef struct
	{
		int size;
		PendingNotification* ntf;
	}PendingNotificationList;


	typedef enum {
		ERR_REQUEST_FAILED = 0x01,
		ERR_INITIATEAUTHENTICATION_FAILED = 0x02,
		ERR_AUTHENTICATECLIENT_FAILED = 0x03,
		ERR_GETBOUNDPROFILEPACKAGE_FAILED = 0x04,
		ERR_HANDLENOTIFICATION_FAILED = 0x05,
		ERR_CANCELSESSION_FAILED = 0x06,
		ERR_CARD_RETURN_ERROR = 0x07,
		ERR_GETPENDINGNOTIFICATION = 0x08,
		ERR_BASE64_DECODE = 0x09,
	}ERROR_TYPE;

	typedef enum {
		profileClass_test = 0,
		profileClass_blank = 1,
		profileClass_operational = 2,
	}ProfileClass;

	typedef enum {
		euiccInfo1 = 1,
		euiccInfo2 = 2,
	}EuiccInfoType;

	typedef enum {
		delete_operational = 0,
		delete_test = 1,
	}ResetOptions;

	typedef enum {
		refresh_not_set,
		refresh_set,
		opr_mark_a1,
		opr_mark_a2,
		opr_mark_a3,
		opr_mark_null,
	}ProfileOptParam;


int TyDownloadDpAddress(Lpa_Ctx *ctx);
int TyProfileDownload(Lpa_Ctx *ctx, char * ctxParams1, char * ctxParams2, char * ctxParams3);

int TyGetProfilesInfo(Lpa_Ctx *ctx, ProfileInfoList* list, ProfileClass profileClass);
void TyFreeProfileInfoList(ProfileInfoList* list);

int TyGetEUICCChallenge(Lpa_Ctx *ctx, char** mResp);
int TyGetEUICCInfo(Lpa_Ctx *ctx, EuiccInfoType euiccInfo1Or2,char** dest);
int TyGetEID(Lpa_Ctx *ctx, char** mResp);
int TySetNickname(Lpa_Ctx *ctx, char* iccid ,char* nickNmae);
int TySetDefaultDpAddress(Lpa_Ctx *ctx, char* dpAddress);
int TyGetEuiccConfiguredAddresses(Lpa_Ctx *ctx, int type,char** resp);
int TyRetrieveNotificationsList(Lpa_Ctx *ctx, PendingNotificationList* mList);
int TyRemoveNotificationFromList(Lpa_Ctx *ctx, char* notifyCounter);
void TyFreePendingNotification(PendingNotification* pt);
void TyFreePendingNotificationList(PendingNotificationList* list);

int TyEnable(Lpa_Ctx *ctx, char* iccidOrISDPaid, ProfileOptParam refreshflag);
int TyDisable(Lpa_Ctx *ctx, char* iccidOrISDPaid, ProfileOptParam refreshflag);
int TyDeleteProfile(Lpa_Ctx *ctx, char* iccid, ProfileOptParam param);
int TyEUICCMemoryReset(Lpa_Ctx *ctx, ResetOptions resetOptions);

int TyGetRAT(Lpa_Ctx *ctx, RATBeanList* ratList);
void TyFreeRATBean(RATBean* ratBean);
void TyFreeRATBeanList(RATBeanList* list);

const char *TyProfileDownloadError(Lpa_Ctx *ctx);
const char *TyProfileDownloadIccid(Lpa_Ctx *ctx);
const char *TyLpaErrMsg(lpa_result_t errcode);

#ifdef __cplusplus
}
#endif

#endif