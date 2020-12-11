#include "memleak.h"
#include "json_util.h"
#include "ty_mqtt_basic_info.h"
#include <stdlib.h>
#include <stdio.h>
#include "ty_os_adpt.h"


//解析下行任务信息
int ty_cloud_task_down_parser (char *jsonStr, ty_cloudmsg_task_down *task){
	cJSON *jsonRoot;
	int rc = 0;
    LOG(" %s %d  \r\n",__FUNCTION__,__LINE__);
	if (NULL == jsonStr || NULL == task){
		return -1;
	}
	 
	jsonRoot = cJSON_Parse(jsonStr);
	if (NULL == jsonRoot){
		LOG("Error before: [%s]\r\n", cJSON_GetErrorPtr());
		return -2;
	}

	if (!cjson_item_str(jsonRoot, "id", (char**)&(task->id))){
		rc = -3;
		goto exit;

	}
	
	if (!cjson_item_str(jsonRoot, "version", (char**)&(task->version))){
		rc = -4;
		goto exit;

	}

	cJSON *jsonParams = cJSON_GetObjectItem(jsonRoot, "params");
	if (NULL == jsonParams || cJSON_Object != jsonParams->type){
		rc = -5;
		goto exit;
	}

	if (!cjson_item_str(jsonParams, "task_no", (char**)&(task->task_No))){
		rc = -6;
		goto exit;

	}

	char *taskTypeStr = NULL;
	if (!cjson_item_str(jsonParams, "task_type", (char**)&taskTypeStr)){
		rc = -7;
		goto exit;

	}
	task->task_type = atoi(taskTypeStr);

	cJSON *jsonTransData = cJSON_GetObjectItem(jsonParams, "trans_data");
	if (NULL == jsonTransData || cJSON_Object != jsonTransData->type){
		rc = -8;
		goto exit;
	}
	
	cJSON *jsonFotaParam;
	cJSON *jsonSAMParam;
	cJSON *jsonTSMParam;
	switch(task->task_type){
		case 1:
			{
			
				jsonFotaParam = cJSON_GetObjectItem(jsonTransData, "fota_param");
				if (NULL == jsonFotaParam || cJSON_Object != jsonFotaParam->type){
					rc = -9;
					goto exit;
				}
				if (!cjson_item_str(jsonFotaParam, "start_version", (char**)&(task->params.fota.start_version))){
					rc = -10;
					goto exit;
				
				}

				if (!cjson_item_str(jsonFotaParam, "target_version", (char**)&(task->params.fota.target_version))){
					rc = -11;
					goto exit;
				
				}

				if (!cjson_item_str(jsonFotaParam, "fota_url", (char**)&(task->params.fota.fota_url))){
					rc = -12;
					goto exit;
				
				}
			    if (!cjson_item_str(jsonFotaParam, "modrsp_url", &(task->params.fota.modrsp_url))){
					rc = -26;
					goto exit;
				
				}
				char *signTypeStr = NULL;
			    if (!cjson_item_str(jsonFotaParam, "sign_type", (char**)&signTypeStr)){
					rc = -13;
					goto exit;
				
				}
				task->params.fota.sign_type = atoi(signTypeStr);
								
				
				if (!cjson_item_str(jsonFotaParam, "sign_value", (char**)&(task->params.fota.sign_value))){
					rc = -14;
					goto exit;
				
				}

				char *retryStrategy = NULL;

				if (!cjson_item_str(jsonFotaParam, "retry_strategy", (char**)&retryStrategy)){
					rc = -15;
					goto exit;
				
				}
				task->params.fota.retry_strategy = atoi(retryStrategy);
			}
			break;
		case 2:
		case 3:
			{
				jsonSAMParam = cJSON_GetObjectItem(jsonTransData, "sam_param");
				if (NULL == jsonSAMParam || cJSON_Object != jsonSAMParam->type){
					rc = -16;
					goto exit;
				}
				if (!cjson_item_str(jsonSAMParam, "sam_url", (char**)&(task->params.sam.sam_url))){
					rc = -17;
					goto exit;
				
				}
				if (!cjson_item_str(jsonSAMParam, "session_key", (char**)&(task->params.sam.session_key))){
					rc = -18;
					goto exit;
				
				}
				char *retryStrategy = NULL;
				if (!cjson_item_str(jsonSAMParam, "retry_strategy", (char**)&retryStrategy)){
					rc = -19;
					goto exit;
				
				}
				task->params.sam.retry_strategy = atoi(retryStrategy);
				if (!cjson_item_str(jsonSAMParam, "app_aid", (char**)&(task->params.sam.app_aid))){
					rc = -20;
					goto exit;
				
				}
			}
			break;
		case 4:
		case 5:
		case 6:
		case 7:
			{
				jsonTSMParam = cJSON_GetObjectItem(jsonTransData, "tsm_param");
				if (NULL == jsonTSMParam || cJSON_Object != jsonTSMParam->type){
					rc = -21;
					goto exit;
				}
			   if (!cjson_item_str(jsonTSMParam, "app_aid", (char**)&(task->params.tsm.app_aid))){
					rc = -22;
					goto exit;
				
				}
				if (!cjson_item_str(jsonTSMParam, "tsm_url", (char**)&(task->params.tsm.tsm_url))){
					rc = -23;
					goto exit;
				
				}
				if (!cjson_item_str(jsonTSMParam, "session_key", (char**)&(task->params.tsm.session_key))){
					rc = -24;
					goto exit;
				
				}
				
				char *retryStrategy = NULL;
				if (!cjson_item_str(jsonTSMParam, "retry_strategy", (char**)&retryStrategy)){
					rc = -25;
					goto exit;
				
				}
				task->params.tsm.retry_strategy = atoi(retryStrategy);
			}
			break;
		default:
			break;
	}

	
exit:
	if (rc == 0){
		task->root = jsonRoot;
        LOG("parser OK,ret=%d\r\n",rc);
	} else {
	    LOG("parser error,ret=%d\r\n",rc);
        LOG("ty_cloud_task_down_parser Error: [%s]\r\n",cJSON_GetErrorPtr());
		cJSON_Delete(jsonRoot);
	}
	return rc;
	
}

//解析下行全量响应信息
int ty_cloud_report_down_parser (char *jsonStr, ty_cloudmsg_report_down *down){
	cJSON *jsonRoot, *jsonValue;
	int rc = 0;

	if (NULL == jsonStr || NULL == down){
		return -1;
	}
	 
	jsonRoot = cJSON_Parse(jsonStr);
	if (NULL == jsonRoot){
		LOG("Error before: [%s]\r\n", cJSON_GetErrorPtr());
		return -2;
	}

	if (!cjson_item_str(jsonRoot, "id", &(down->id))){
		rc = -3;
		goto exit;

	}
	if (!cjson_item_str(jsonRoot, "version", &(down->version))){
		rc = -4;
		goto exit;

	}
	if (!cjson_item_str(jsonRoot, "code", &(down->code))){
		rc = -5;
		goto exit;

	}
	
	cJSON *jsonData = cJSON_GetObjectItem(jsonRoot, "data");
	if (NULL == jsonData || cJSON_Object != jsonData->type){
		rc = -6;
		goto exit;
	}
	
	cJSON *jsonModuleInfo = cJSON_GetObjectItem(jsonData, "module_inf");
	if (NULL == jsonModuleInfo || cJSON_Object != jsonModuleInfo->type){
		rc = -7;
		goto exit;
	}
	
	if (!cjson_item_str(jsonModuleInfo, "product_type", &(down->module_inf.product_type))){
		rc = -8;
		goto exit;

	}
	
	if (!cjson_item_str(jsonModuleInfo, "bind_imei", &(down->module_inf.bind_imei))){
		rc = -9;
		goto exit;

	}
	
	if (!cjson_item_str(jsonModuleInfo, "firmware_version", &(down->module_inf.firmware_version))){
		rc = -10;
		goto exit;

	}
		
	cJSON *jsonModuleSIM = cJSON_GetObjectItem(jsonData, "module_sim");
	if (NULL == jsonModuleSIM || cJSON_Object != jsonModuleSIM->type){
		rc = -11;
		goto exit;
	}

	cJSON *jsonCodeNo = cJSON_GetObjectItem(jsonModuleSIM, "code_no");
	cJSON *jsonArray;
	//code_no 非必传
	if (NULL != jsonCodeNo && cJSON_Array == jsonCodeNo->type){
		
		int size = cJSON_GetArraySize(jsonCodeNo);
		down->module_SIM.size = size;
		down->module_SIM.code_no_list = lpa_malloc(sizeof(ty_code_No)*size);
		if (NULL == down->module_SIM.code_no_list){
			rc = -12;
			goto exit;
		}
		
		memset(down->module_SIM.code_no_list, 0, sizeof(ty_code_No)*size);
		
		int i = 0;
		for (i = 0; i < size; i++){
			
			jsonArray = cJSON_GetArrayItem(jsonCodeNo, i);
			if (NULL == jsonArray || cJSON_Object != jsonArray->type){
				rc = -13;
				goto exit;
			}
			if (!cjson_item_str(jsonArray, "bind_iccid", &(down->module_SIM.code_no_list[i].bind_iccid))){
				rc = -14;
				goto exit;

			}
			
			cJSON *jsonWriteIccid = cJSON_GetObjectItem(jsonArray, "write_iccid");
			if (NULL != jsonWriteIccid && cJSON_IsBool(jsonWriteIccid)){
				down->module_SIM.code_no_list[i].write_iccid = jsonWriteIccid->valueint;
			}

			cJSON *jsonUseIccid = cJSON_GetObjectItem(jsonArray, "use_iccid");
			if (NULL != jsonUseIccid && cJSON_IsBool(jsonUseIccid)){
				down->module_SIM.code_no_list[i].use_iccid = jsonUseIccid->valueint;
			}
			
		}
	}
	
	cJSON *jsonModuleSETSM = cJSON_GetObjectItem(jsonData, "module_se_tsm");
	if (NULL == jsonModuleSETSM || cJSON_Object != jsonModuleSETSM->type){
		rc = -15;
		goto exit;
	}
	
	if (!cjson_item_str(jsonModuleSETSM, "seid", &(down->module_SE_TSM.SEID))){
		//rc = -16;
		//goto exit;
		LOG("WARNNIG!!!  no SEID info!!!!!!\r\n");

	}
	
	cjson_item_int(jsonModuleSETSM, "rest_space", &(down->module_SE_TSM.rest_space));

	cJSON *jsonAIDList = cJSON_GetObjectItem(jsonModuleSETSM, "AID");
	if (NULL != jsonAIDList && cJSON_Array == jsonAIDList->type){
		//rc = -17;
		//goto exit;
	

    	int n = cJSON_GetArraySize(jsonAIDList);
    	down->module_SE_TSM.AID_size = n;
    	down->module_SE_TSM.AID_list = lpa_malloc(sizeof(ty_AID)*n);
    	if (NULL == down->module_SE_TSM.AID_list){
    		rc = -18;
    		goto exit;
    	}
    	
    	memset(down->module_SE_TSM.AID_list, 0, sizeof(ty_AID)*n);
    	int j = 0;
    	for (j = 0; j < n; j++){
    				jsonArray = cJSON_GetArrayItem(jsonAIDList, j);
    		if (NULL == jsonArray || cJSON_Object != jsonArray->type){
    			rc = -19;
    			goto exit;
    		}
    		
    		if (!cjson_item_str(jsonArray, "app_aid", &(down->module_SE_TSM.AID_list[j].app_aid))){
    			rc = -20;
    			goto exit;

    		}
    		
    		if (!cjson_item_str(jsonArray, "app_status", &(down->module_SE_TSM.AID_list[j].app_status))){
    			rc = -21;
    			goto exit;

    		}
    		
    		if (!cjson_item_str(jsonArray, "app_version", &(down->module_SE_TSM.AID_list[j].app_version))){
    			rc = -22;
    			goto exit;

    		}
    		
    		if (!cjson_item_str(jsonArray, "app_life_cycle_state", &(down->module_SE_TSM.AID_list[j].app_life_cycle_state))){
    			rc = -23;
    			goto exit;

    		}
    		
    	}
	}
    else
    {
        LOG("WARNNIG!!!  no AID info!!!!!!\r\n");
    }
exit:
	if (0 != rc){
        LOG("ty_cloud_report_down_parser Error: [%s]\r\n",cJSON_GetErrorPtr());
		if (NULL != down->module_SIM.code_no_list){
			lpa_free(down->module_SIM.code_no_list);
			down->module_SIM.code_no_list = NULL;
		}

		if (NULL != down->module_SE_TSM.AID_list){
			lpa_free(down->module_SE_TSM.AID_list);
			down->module_SE_TSM.AID_list = NULL;
		}
		cJSON_Delete(jsonRoot);
	}

	if (rc == 0){
		down->root = jsonRoot;
	}
	return rc;
}


//生成上行任务信息
int ty_cloud_task_up_build (char **json, int *json_len, ty_cloudmsg_task_up *up){
	int rc = 0;
    char *str = NULL;
	if (NULL == json || NULL == up){
		return -1;
	}

	cJSON *jsonRoot, *jsonData, *jsonModuleInf, *jsonModuleSETSM;

	jsonRoot = cJSON_CreateObject();
	
	if (NULL == jsonRoot){
		rc = -2;
		goto exit;
	}
	cJSON_AddItemToObject(jsonRoot, "id", cJSON_CreateString(up->id));
	cJSON_AddItemToObject(jsonRoot, "version", cJSON_CreateString(up->version));
	cJSON_AddItemToObject(jsonRoot, "code", cJSON_CreateString(up->code));

	jsonData = cJSON_CreateObject();
	if (NULL == jsonData){
		rc = -3;
		goto exit;
	}

	cJSON_AddItemToObject(jsonRoot, "data", jsonData);


	cJSON_AddItemToObject(jsonData, "task_no", cJSON_CreateString(up->task_no));
	char taskTypeStr[10] = {0};
	sprintf(taskTypeStr, "%d", up->task_type);
	cJSON_AddItemToObject(jsonData, "task_type", cJSON_CreateString(taskTypeStr));
	
	cJSON_AddItemToObject(jsonData, "fota_msg", cJSON_CreateBool(up->fota_msg));
	
	cJSON_AddItemToObject(jsonData, "sam_tsm_msg", cJSON_CreateBool(up->sam_tsm_msg));

	if (1 == up->task_type && up->fota_msg){
		jsonModuleInf = cJSON_CreateObject();
		if (NULL == jsonModuleInf){
			rc = -4;
			goto exit;
		}
		cJSON_AddItemToObject(jsonData, "module_inf", jsonModuleInf);
		cJSON_AddItemToObject(jsonModuleInf, "product_type", cJSON_CreateString(up->msg.module_inf.product_type));
		cJSON_AddItemToObject(jsonModuleInf, "bind_imei", cJSON_CreateString(up->msg.module_inf.bind_imei));
		cJSON_AddItemToObject(jsonModuleInf, "firmware_version", cJSON_CreateString(up->msg.module_inf.firmware_version));
	} else if (up->task_type >= 2 && up->task_type <= 7 && up->sam_tsm_msg){
		jsonModuleSETSM = cJSON_CreateObject();
		if (NULL == jsonModuleSETSM){
			rc = -5;
			goto exit;
		}
		cJSON_AddItemToObject(jsonData, "module_se_tsm", jsonModuleSETSM);
		if (up->task_type >= 4 && up->task_type <= 7){
			cJSON_AddItemToObject(jsonModuleSETSM, "seid", cJSON_CreateString(up->msg.module_SE_TSM.SEID));
			cJSON_AddItemToObject(jsonModuleSETSM, "rest_space", cJSON_CreateNumber(up->msg.module_SE_TSM.rest_space));
		} 
		if (up->msg.module_SE_TSM.AID_size > 0){
			
			cJSON *jsonArray = cJSON_CreateArray();
			if (NULL == jsonArray){
				rc = -6;
				return;
			}
			cJSON_AddItemToObject(jsonModuleSETSM, "aid", jsonArray);
			int i = 0;
			for (i = 0; i < up->msg.module_SE_TSM.AID_size; i++){
				
				cJSON *jsonArrayItem = cJSON_CreateObject();
				if (NULL == jsonArrayItem){
					rc = -7;
					goto exit;
				}
				cJSON_AddItemToObject(jsonArrayItem, "app_aid", cJSON_CreateString(up->msg.module_SE_TSM.AID_list[i].app_aid));
				cJSON_AddItemToObject(jsonArrayItem, "app_status", cJSON_CreateString(up->msg.module_SE_TSM.AID_list[i].app_status));
				cJSON_AddItemToObject(jsonArrayItem, "app_version", cJSON_CreateString(up->msg.module_SE_TSM.AID_list[i].app_version));
				cJSON_AddItemToObject(jsonArrayItem, "app_life_cycle_state", cJSON_CreateString(up->msg.module_SE_TSM.AID_list[i].app_life_cycle_state));

				cJSON_AddItemToArray(jsonArray, jsonArrayItem);
			}
		}
		
	}
	str = cJSON_PrintUnformatted(jsonRoot);
	
	LOG("construct json success \r\n");
    ty_printf_hexbuf (str, strlen(str));
    LOG("len is %d\r\n", *json_len);
	if (NULL != str){
		*json = lpa_malloc(strlen(str)+ 1);
		if (NULL == json){
			rc = -8;
		}
		memcpy(*json, str, strlen(str));
		*json_len = strlen(str);
	}
	LOG("len is %d\r\n", *json_len);
    
exit:
	if (NULL != str){
		lpa_free(str);
		str = NULL;
	}
	cJSON_Delete(jsonRoot);
	return rc;
}

//生成上行全量上报信息
int ty_cloud_report_up_build(char **json,int *json_len,ty_cloudmsg_report_up *report){

	int rc = 0;
    char *str = NULL;
	if (NULL == json || NULL == report){
		return -1;
	}

	cJSON *jsonRoot, *jsonParams, *jsonModuleInf, *jsonModuleSIM, *jsonModuleSETSM;

	jsonRoot = cJSON_CreateObject();
	if (NULL == jsonRoot){
		rc = -2;
		goto exit;
	}

	cJSON_AddItemToObject(jsonRoot, "id", cJSON_CreateString(report->id));
	cJSON_AddItemToObject(jsonRoot, "version", cJSON_CreateString(report->version));

	jsonParams = cJSON_CreateObject();
	if (NULL == jsonParams){
		rc = -3;
		goto exit;
	}
	cJSON_AddItemToObject(jsonRoot, "params", jsonParams);

	jsonModuleInf = cJSON_CreateObject();
	jsonModuleSIM = cJSON_CreateObject();
	jsonModuleSETSM = cJSON_CreateObject();
	if (NULL == jsonModuleInf || NULL == jsonModuleSIM || NULL == jsonModuleSETSM){
		rc = -4;
		goto exit;

	}


	cJSON_AddItemToObject(jsonParams, "module_inf", jsonModuleInf);
	cJSON_AddItemToObject(jsonParams, "module_sim", jsonModuleSIM);
	cJSON_AddItemToObject(jsonParams, "module_se_tsm", jsonModuleSETSM);

	cJSON_AddItemToObject(jsonModuleInf, "product_type", cJSON_CreateString(report->module_inf.product_type));
	cJSON_AddItemToObject(jsonModuleInf, "bind_imei", cJSON_CreateString(report->module_inf.bind_imei));
	cJSON_AddItemToObject(jsonModuleInf, "firmware_version", cJSON_CreateString(report->module_inf.firmware_version));

	if (report->module_SIM.size > 0){
		cJSON *jsonArray = cJSON_CreateArray();
		if (NULL == jsonArray){
			rc = -5;
			return rc;
		}
		cJSON_AddItemToObject(jsonModuleSIM, "code_no", jsonArray);

		int i = 0; 
		for (i = 0; i < report->module_SIM.size; i++){
			cJSON *jsonArrayItem = cJSON_CreateObject();
			if (NULL == jsonArrayItem){
				rc = -6;
				goto exit;
			}
			cJSON_AddItemToObject(jsonArrayItem, "bind_iccid", cJSON_CreateString(report->module_SIM.code_no_list[i].bind_iccid));
			cJSON_AddItemToObject(jsonArrayItem, "write_iccid", cJSON_CreateBool(report->module_SIM.code_no_list[i].write_iccid));
			cJSON_AddItemToObject(jsonArrayItem, "use_iccid", cJSON_CreateBool(report->module_SIM.code_no_list[i].use_iccid));

			cJSON_AddItemToArray(jsonArray, jsonArrayItem);
		}
	}
    else
    {
        LOG("WARNNIG!!!  no ICCID info!!!!!!\r\n");
    }


	if (NULL != report->module_SE_TSM.SEID){
		cJSON_AddItemToObject(jsonModuleSETSM, "seid", cJSON_CreateString(report->module_SE_TSM.SEID));
	}

	if (report->module_SE_TSM.rest_space >= 0){
		cJSON_AddItemToObject(jsonModuleSETSM, "rest_space", cJSON_CreateNumber(report->module_SE_TSM.rest_space));
	}

	if (report->module_SE_TSM.AID_size > 0){
		cJSON *jsonArray = cJSON_CreateArray();
		if (NULL == jsonArray){
			rc = -7;
			return rc;
		}
		cJSON_AddItemToObject(jsonModuleSETSM, "aid", jsonArray);

		int i = 0; 
		for (i = 0; i < report->module_SE_TSM.AID_size; i++){
			cJSON *jsonArrayItem = cJSON_CreateObject();
			if (NULL == jsonArrayItem){
				rc = -8;
				goto exit;
			}
			cJSON_AddItemToObject(jsonArrayItem, "app_aid", cJSON_CreateString(report->module_SE_TSM.AID_list[i].app_aid));

			cJSON_AddItemToObject(jsonArrayItem, "app_status", cJSON_CreateString(report->module_SE_TSM.AID_list[i].app_status));
			cJSON_AddItemToObject(jsonArrayItem, "app_version", cJSON_CreateString(report->module_SE_TSM.AID_list[i].app_version));
			cJSON_AddItemToObject(jsonArrayItem, "app_life_cycle_state", cJSON_CreateString(report->module_SE_TSM.AID_list[i].app_life_cycle_state));
			cJSON_AddItemToArray(jsonArray, jsonArrayItem);
		}
	} else {
		LOG("WARNNIG!!! NV flash no AID info!!!!!!\r\n");
		//rc = -9;
		//goto exit;
		
	}

	str = cJSON_PrintUnformatted(jsonRoot);
	if (NULL != str){
		*json = lpa_malloc(strlen(str)+ 1);
		if (NULL == json){
			rc = -10;
		}
		memcpy(*json, str, strlen(str));
		*json_len = strlen(str);
        LOG("construct json success \r\n");
        ty_printf_hexbuf (str, strlen(str));
        LOG("len is %d\r\n", *json_len);
	}
    else
    {
        LOG("construct json FAIL \r\n");
    }
	
exit:
    LOG("%s rc=%d\r\n",__FUNCTION__, rc);
	if (NULL != str){
		lpa_free(str);
		str = NULL;
	}
	cJSON_Delete(jsonRoot);
	return rc;
	
}

int ty_cloud_offline_up_build(char **json,int *json_len,ty_cloudmsg_offline_up *offline)
{
	int rc = 0;
    char *str = NULL;
	if (NULL == json || NULL == offline){
		return -1;
	}

	cJSON *jsonRoot, *jsonParams;

	jsonRoot = cJSON_CreateObject();
	if (NULL == jsonRoot){
		rc = -2;
		goto exit;
	}

	cJSON_AddItemToObject(jsonRoot, "id", cJSON_CreateString(offline->id));
	cJSON_AddItemToObject(jsonRoot, "version", cJSON_CreateString(offline->version));

	jsonParams = cJSON_CreateObject();
	if (NULL == jsonParams){
		rc = -3;
		goto exit;
	}
	cJSON_AddItemToObject(jsonRoot, "params", jsonParams);
    cJSON_AddItemToObject(jsonParams, "product_type", cJSON_CreateNumber(offline->type));
	str = cJSON_PrintUnformatted(jsonRoot);
	if (NULL != str){
		*json = lpa_malloc(strlen(str)+ 1);
		if (NULL == json){
			rc = -10;
		}
		memcpy(*json, str, strlen(str));
		*json_len = strlen(str);
	}
    LOG("construct offline:\r\n");
    ty_printf_hexbuf (str, strlen(str));
	LOG("len is %d\r\n", *json_len);

exit:
        LOG("%s rc=%d\r\n",__FUNCTION__, rc);
        if (NULL != str){
            lpa_free(str);
            str = NULL;
        }
        if(jsonRoot)
            cJSON_Delete(jsonRoot);
    return rc;
}

