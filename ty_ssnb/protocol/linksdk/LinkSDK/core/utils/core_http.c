/**
 * @file aiot_http_api.c
 * @brief HTTP模块实现, 其中包含了向物联网平台认证和上报数据的API接口
 * @date 2019-12-27
 *
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 *
 */

#include "core_http.h"
#define lk_msleep 
#define LK_LOG 


static void _core_http_exec_inc(core_http_handle_t *http_handle)
{
    http_handle->sysdep->core_sysdep_mutex_lock(http_handle->data_mutex);
    http_handle->core_exec_count++;
    http_handle->sysdep->core_sysdep_mutex_unlock(http_handle->data_mutex);
}

static void _core_http_exec_dec(core_http_handle_t *http_handle)
{
    http_handle->sysdep->core_sysdep_mutex_lock(http_handle->data_mutex);
    http_handle->core_exec_count--;
    http_handle->sysdep->core_sysdep_mutex_unlock(http_handle->data_mutex);
}

static int32_t _core_http_sysdep_return(int32_t sysdep_code, int32_t core_code)
{
    if (sysdep_code >= (STATE_PORT_BASE - 0x00FF) && sysdep_code < (STATE_PORT_BASE)) {
        return sysdep_code;
    } else {
        return core_code;
    }
}

static int32_t _core_http_connect(core_http_handle_t *http_handle)
{
    int32_t res = STATE_SUCCESS;

    /* disconnect first if network is already established */
    if (http_handle->network_handle != NULL) {
        http_handle->sysdep->core_sysdep_network_deinit(&http_handle->network_handle);
    }

    /* establish network connection */
    core_sysdep_socket_type_t socket_type = CORE_SYSDEP_SOCKET_TCP_CLIENT;

    if (http_handle->host == NULL) {
        return STATE_USER_INPUT_MISSING_HOST;
    }

    http_handle->network_handle = http_handle->sysdep->core_sysdep_network_init();
    if (http_handle->network_handle == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    if ((res = http_handle->sysdep->core_sysdep_network_setopt(http_handle->network_handle, CORE_SYSDEP_NETWORK_SOCKET_TYPE,
               &socket_type)) < 0 ||
        (res = http_handle->sysdep->core_sysdep_network_setopt(http_handle->network_handle, CORE_SYSDEP_NETWORK_HOST,
                http_handle->host)) < 0 ||
        (res = http_handle->sysdep->core_sysdep_network_setopt(http_handle->network_handle, CORE_SYSDEP_NETWORK_PORT,
                &http_handle->port)) < 0 ||
        (res = http_handle->sysdep->core_sysdep_network_setopt(http_handle->network_handle,
                CORE_SYSDEP_NETWORK_CONNECT_TIMEOUT_MS,
                &http_handle->connect_timeout_ms)) < 0) {
        http_handle->sysdep->core_sysdep_network_deinit(&http_handle->network_handle);
        return _core_http_sysdep_return(res, STATE_SYS_DEPEND_NWK_INVALID_OPTION);
    }

    if (http_handle->cred != NULL) {
        LOG("%s, %d", __func__, __LINE__);
        res = http_handle->sysdep->core_sysdep_network_setopt(http_handle->network_handle, CORE_SYSDEP_NETWORK_CRED,
                http_handle->cred);
        if (res < STATE_SUCCESS) {
            http_handle->sysdep->core_sysdep_network_deinit(&http_handle->network_handle);
            return _core_http_sysdep_return(res, STATE_SYS_DEPEND_NWK_INVALID_CRED);
        }
    }
    LOG("%s, %d\r\n", __func__, __LINE__);
    res = http_handle->sysdep->core_sysdep_network_establish(http_handle->network_handle);
    if (res < STATE_SUCCESS) {
        http_handle->sysdep->core_sysdep_network_deinit(&http_handle->network_handle);
        return _core_http_sysdep_return(res, STATE_SYS_DEPEND_NWK_EST_FAILED);
    }

    return STATE_SUCCESS;
}

static int32_t _core_http_send(core_http_handle_t *http_handle, uint8_t *buffer, uint32_t len, uint32_t timeout_ms)
{
    int32_t res = STATE_SUCCESS;

    if (http_handle->network_handle != NULL) {
        res = http_handle->sysdep->core_sysdep_network_send(http_handle->network_handle, buffer, len, timeout_ms, NULL);
        if (res < STATE_SUCCESS) {
            http_handle->sysdep->core_sysdep_network_deinit(&http_handle->network_handle);
            core_log(http_handle->sysdep, STATE_HTTP_LOG_DISCONNECT, "HTTP network error when sending data, disconnect\r\n");
            res = _core_http_sysdep_return(res, STATE_SYS_DEPEND_NWK_SEND_ERR);
        } else if (res != len) {
            res = STATE_SYS_DEPEND_NWK_WRITE_LESSDATA;
        }
    } else {
        res = STATE_SYS_DEPEND_NWK_CLOSED;
    }

    return res;
}

static int32_t _core_http_send_body(core_http_handle_t *http_handle, uint8_t *content, uint32_t len)
{
    int32_t res = STATE_SUCCESS;

    core_log_hexdump(STATE_HTTP_LOG_SEND_CONTENT, '>', content, len);

    http_handle->sysdep->core_sysdep_mutex_lock(http_handle->send_mutex);
    res = _core_http_send(http_handle, content, len, http_handle->send_timeout_ms);
    http_handle->sysdep->core_sysdep_mutex_unlock(http_handle->send_mutex);

    return res;
}

static void _core_http_header_print(core_http_handle_t *http_handle, char *prefix, char *header, uint32_t header_len)
{
    char *prev = header, *pos = header;
    uint32_t line_len = 0;
    while (pos != header + header_len) {
        if (*(pos) == '\n' && *(pos - 1) == '\r') {
            line_len = (uint32_t)(pos - prev + 1);
            core_log3(http_handle->sysdep, STATE_HTTP_LOG_SEND_HEADER, "%s %.*s", prefix, &line_len, prev);
            prev = pos + 1;
        }
        pos++;
    }
}

static int32_t _core_http_send_header(core_http_handle_t *http_handle, char *method, char *path, char *host,
                                      char *header, char *content_lenstr)
{
    int32_t res = STATE_SUCCESS;
    char *combine_header = NULL;
    char *combine_header_src[] = { method, path, host, header, content_lenstr};
    uint32_t combine_header_len = 0;

    res = core_sprintf(http_handle->sysdep, &combine_header, "%s %s HTTP/1.1\r\nHost: %s\r\n%sContent-Length: %s\r\n\r\n",
                       combine_header_src, sizeof(combine_header_src) / sizeof(char *), CORE_HTTP_MODULE_NAME);
    if (res < STATE_SUCCESS) {
        return res;
    }
    combine_header_len = (uint32_t)strlen(combine_header);

    _core_http_header_print(http_handle, ">", combine_header, combine_header_len);

    http_handle->sysdep->core_sysdep_mutex_lock(http_handle->send_mutex);
    res = _core_http_send(http_handle, (uint8_t *)combine_header, combine_header_len, http_handle->send_timeout_ms);
    http_handle->sysdep->core_sysdep_mutex_unlock(http_handle->send_mutex);

    http_handle->sysdep->core_sysdep_free(combine_header);

    return res;
}

void *core_http_init(void)
{
    core_http_handle_t *http_handle = NULL;
    aiot_sysdep_portfile_t *sysdep = NULL;

    sysdep = aiot_sysdep_get_portfile();
    if (sysdep == NULL) {
        return NULL;
    }

    http_handle = sysdep->core_sysdep_malloc(sizeof(core_http_handle_t), CORE_HTTP_MODULE_NAME);
    if (http_handle == NULL) {
        return NULL;
    }
    memset(http_handle, 0, sizeof(core_http_handle_t));

    http_handle->sysdep = sysdep;
    http_handle->connect_timeout_ms = CORE_HTTP_DEFAULT_CONNECT_TIMEOUT_MS;
    http_handle->send_timeout_ms = CORE_HTTP_DEFAULT_SEND_TIMEOUT_MS;
    http_handle->recv_timeout_ms = CORE_HTTP_DEFAULT_RECV_TIMEOUT_MS;
    http_handle->header_line_max_len = CORE_HTTP_DEFAULT_HEADER_LINE_MAX_LEN;
    http_handle->body_buffer_max_len = CORE_HTTP_DEFAULT_BODY_MAX_LEN;
    http_handle->deinit_timeout_ms = CORE_HTTP_DEFAULT_DEINIT_TIMEOUT_MS;

    http_handle->data_mutex = http_handle->sysdep->core_sysdep_mutex_init();
    http_handle->send_mutex = http_handle->sysdep->core_sysdep_mutex_init();
    http_handle->recv_mutex = http_handle->sysdep->core_sysdep_mutex_init();

    http_handle->core_exec_enabled = 1;

    return http_handle;
}

int32_t core_http_setopt(void *handle, core_http_option_t option, void *data)
{
    int32_t res = STATE_SUCCESS;
    core_http_handle_t *http_handle = (core_http_handle_t *)handle;

    if (http_handle == NULL || data == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (option >= CORE_HTTPOPT_MAX) {
        return STATE_USER_INPUT_OUT_RANGE;
    }

    if (http_handle->core_exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _core_http_exec_inc(http_handle);

    http_handle->sysdep->core_sysdep_mutex_lock(http_handle->data_mutex);
    switch (option) {
        case CORE_HTTPOPT_HOST: {
            res = core_strdup(http_handle->sysdep, &http_handle->host, (char *)data, CORE_HTTP_MODULE_NAME);
        }
        break;
        case CORE_HTTPOPT_PORT: {
            http_handle->port = *(uint16_t *)data;
        }
        break;
        case CORE_HTTPOPT_NETWORK_CRED: {
            if (http_handle->cred != NULL) {
                http_handle->sysdep->core_sysdep_free(http_handle->cred);
                http_handle->cred = NULL;
            }
            http_handle->cred = http_handle->sysdep->core_sysdep_malloc(sizeof(aiot_sysdep_network_cred_t), CORE_HTTP_MODULE_NAME);
            if (http_handle->cred != NULL) {
                memset(http_handle->cred, 0, sizeof(aiot_sysdep_network_cred_t));
                memcpy(http_handle->cred, data, sizeof(aiot_sysdep_network_cred_t));
            } else {
                res = STATE_SYS_DEPEND_MALLOC_FAILED;
            }

        }
        break;
        case CORE_HTTPOPT_CONNECT_TIMEOUT_MS: {
            http_handle->connect_timeout_ms = *(uint32_t *)data;
        }
        break;
        case CORE_HTTPOPT_SEND_TIMEOUT_MS: {
            http_handle->send_timeout_ms = *(uint32_t *)data;
        }
        break;
        case CORE_HTTPOPT_RECV_TIMEOUT_MS: {
            http_handle->recv_timeout_ms = *(uint32_t *)data;
        }
        break;
        case CORE_HTTPOPT_DEINIT_TIMEOUT_MS: {
            http_handle->deinit_timeout_ms = *(uint32_t *)data;
        }
        break;
        case CORE_HTTPOPT_HEADER_LINE_MAX_LEN: {
            http_handle->header_line_max_len = *(uint32_t *)data;
        }
        break;
        case CORE_HTTPOPT_BODY_BUFFER_MAX_LEN: {
            http_handle->body_buffer_max_len = *(uint32_t *)data;
        }
        break;
        case CORE_HTTPOPT_EVENT_HANDLER: {
            http_handle->event_handler = (aiot_http_event_handler_t)data;
        }
        break;
        case CORE_HTTPOPT_USERDATA: {
            http_handle->core_userdata = data;
        }
        break;
        case CORE_HTTPOPT_RECV_HANDLER: {
            http_handle->core_recv_handler = (aiot_http_recv_handler_t)data;
        }
        break;
        default: {
            res = STATE_USER_INPUT_UNKNOWN_OPTION;
        }
        break;
    }
    http_handle->sysdep->core_sysdep_mutex_unlock(http_handle->data_mutex);

    _core_http_exec_dec(http_handle);

    return res;
}

int32_t core_http_connect(void *handle)
{
    int32_t res = STATE_SUCCESS;
    core_http_handle_t *http_handle = (core_http_handle_t *)handle;

    if (http_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (http_handle->core_exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _core_http_exec_inc(http_handle);

    /* connect to host */
    http_handle->sysdep->core_sysdep_mutex_lock(http_handle->send_mutex);
    http_handle->sysdep->core_sysdep_mutex_lock(http_handle->recv_mutex);
    LOG("%s, %d\r\n", __func__, __LINE__);
    res = _core_http_connect(http_handle);
    http_handle->sysdep->core_sysdep_mutex_unlock(http_handle->recv_mutex);
    http_handle->sysdep->core_sysdep_mutex_unlock(http_handle->send_mutex);

    _core_http_exec_dec(http_handle);

    return res;
}

int32_t core_http_send(void *handle, const core_http_request_t *request)
{
    int32_t res = STATE_SUCCESS;
    char content_lenstr[11] = {0};
    core_http_handle_t *http_handle = (core_http_handle_t *)handle;

    if (http_handle == NULL || request == NULL ||
        request->path == NULL || request->method == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (http_handle->host == NULL) {
        return STATE_USER_INPUT_MISSING_HOST;
    }

    if (http_handle->network_handle == NULL) {
        return STATE_SYS_DEPEND_NWK_CLOSED;
    }

    if (http_handle->core_exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    memset(&http_handle->session, 0, sizeof(core_http_session_t));

    _core_http_exec_inc(http_handle);

    /* send http header */
    core_uint2str(request->content_len, content_lenstr, NULL);
    res = _core_http_send_header(http_handle, request->method, request->path, http_handle->host, request->header,
                                 content_lenstr);
    if (res < STATE_SUCCESS) {
        _core_http_exec_dec(http_handle);
        return res;
    } else {
        res = STATE_SUCCESS;
    }

    /* send http content */
    if (request->content != NULL && request->content_len > 0) {
        res = _core_http_send_body(http_handle, request->content, request->content_len);
        if (res < STATE_SUCCESS) {
            _core_http_exec_dec(http_handle);
            return res;
        }
    }

    _core_http_exec_dec(http_handle);

    return res;
}

static void _core_http_recv_status_code(core_http_handle_t *http_handle, uint32_t status_code)
{
    aiot_http_recv_t packet;

    if (http_handle->core_recv_handler == NULL) {
        return;
    }

    memset(&packet, 0, sizeof(aiot_http_recv_t));
    packet.type = AIOT_HTTPRECV_STATUS_CODE;
    packet.data.status_code.code = status_code;

    http_handle->core_recv_handler(http_handle, &packet, http_handle->core_userdata);
}

static void _core_http_recv_header_pair(core_http_handle_t *http_handle, char *key, uint32_t key_len, char *value,
                                        uint32_t value_len)
{
    char *pair_key = NULL, *pair_value = NULL;
    aiot_http_recv_t packet;
    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    if (http_handle->core_recv_handler == NULL) {
        return;
    }
    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    pair_key = http_handle->sysdep->core_sysdep_malloc(key_len + 1, CORE_HTTP_MODULE_NAME);
    if (pair_key == NULL) {
        return;
    }
    memset(pair_key, 0, key_len + 1);
    memcpy(pair_key, key, key_len);
    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    pair_value = http_handle->sysdep->core_sysdep_malloc(value_len + 1, CORE_HTTP_MODULE_NAME);
    if (pair_value == NULL) {
        http_handle->sysdep->core_sysdep_free(pair_key);
        return;
    }   
    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    memset(pair_value, 0, value_len + 1);
    memcpy(pair_value, value, value_len);

    packet.type = AIOT_HTTPRECV_HEADER;
    packet.data.header.key = pair_key;
    packet.data.header.value = pair_value;
    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    http_handle->core_recv_handler(http_handle, &packet, http_handle->core_userdata);
    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);

    http_handle->sysdep->core_sysdep_free(pair_key);
    http_handle->sysdep->core_sysdep_free(pair_value);
    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
}

static int32_t _core_http_recv(core_http_handle_t *http_handle, uint8_t *buffer, uint32_t len, uint32_t timeout_ms)
{
    int32_t res = STATE_SUCCESS;

    if (http_handle->network_handle != NULL) {
        res = http_handle->sysdep->core_sysdep_network_recv(http_handle->network_handle, buffer, len, timeout_ms, NULL);
        if (res < STATE_SUCCESS) {
            http_handle->sysdep->core_sysdep_network_deinit(&http_handle->network_handle);
            core_log(http_handle->sysdep, STATE_HTTP_LOG_DISCONNECT, "HTTP network error when receving data, disconnect\r\n");
            res = _core_http_sysdep_return(res, STATE_SYS_DEPEND_NWK_RECV_ERR);
        }
    } else {
        res = STATE_SYS_DEPEND_NWK_CLOSED;
    }

    return res;
}

static int32_t _core_http_recv_header(core_http_handle_t *http_handle, uint32_t *body_total_len, uint32_t *head_type)
{
    int32_t res = STATE_SUCCESS;
    char *line = NULL;
    uint32_t idx = 0, line_max_len = http_handle->header_line_max_len;
    uint64_t timenow_ms = 0;

    line = http_handle->sysdep->core_sysdep_malloc(line_max_len, CORE_HTTP_MODULE_NAME);
    if (line == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }
    memset(line, 0, line_max_len);

    timenow_ms = http_handle->sysdep->core_sysdep_time();
    for (idx = 0; idx < line_max_len;) {
        if (timenow_ms > http_handle->sysdep->core_sysdep_time()) {
            timenow_ms = http_handle->sysdep->core_sysdep_time();
        }
        if (http_handle->sysdep->core_sysdep_time() - timenow_ms >= http_handle->recv_timeout_ms) {
            res =  STATE_HTTP_HEADER_INVALID;
            break;
        }
        if (idx + 2 > line_max_len) {
            res = STATE_HTTP_HEADER_BUFFER_TOO_SHORT;
            break;
        }
        /* read http header, "\r\n" in the end */
        if ((res = _core_http_recv(http_handle, (uint8_t *)&line[idx], 1,
                                   http_handle->recv_timeout_ms)) < STATE_SUCCESS) {
            break;
        }
        idx++;
        if (res == 0 || (line[idx - 1] != '\r')) {
            continue;
        }
        if ((res = _core_http_recv(http_handle, (uint8_t *)&line[idx], 1,
                                   http_handle->recv_timeout_ms)) < STATE_SUCCESS) {
            break;
        }
        idx++;
        if (res == 0 || (line[idx - 1] != '\n')) {
            continue;
        }
        LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
        core_log2(http_handle->sysdep, STATE_HTTP_LOG_RECV_HEADER, "< %.*s", &idx, line);
        LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
        /* next line should be http response body */
        if (idx == 2) {
            break;
        }
        LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
        /* status code */
        if ((idx > (strlen("HTTP/1.1 ") + 3)) && (memcmp(line, "HTTP/1.1 ", strlen("HTTP/1.1 "))) == 0) {
            uint32_t status_code = 0, code_idx = 0;
            for (code_idx = strlen("HTTP/1.1 "); code_idx < idx; code_idx++) {
                if (line[code_idx] < '0' || line[code_idx] > '9') {
                    break;
                }
            }
            LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
            res = core_str2uint(&line[strlen("HTTP/1.1 ")], (code_idx - strlen("HTTP/1.1 ")), &status_code);
            if (res < STATE_SUCCESS) {
                res = STATE_HTTP_STATUS_LINE_INVALID;
                break;
            }
            LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
            _core_http_recv_status_code(http_handle, status_code);
        }
        /* header */
        {
            LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
            uint32_t deli_idx = 0;
            for (deli_idx = 0; deli_idx < idx; deli_idx++) {
                if (line[deli_idx] == ':' && line[deli_idx + 1] == ' ') {
                    if ((deli_idx + 2 == strlen("Content-Length: ")) && (memcmp(line, "Content-Length: ", deli_idx + 2) == 0)) {
                        LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
						//如果同时存在Transfer-Encoding和Content-Length，忽略Content-Length
						if (*head_type == 0) {
                            LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
							core_str2uint(&line[deli_idx + 2], (uint32_t)(idx - deli_idx - 4), body_total_len); //十进制的长度转换
							_core_http_recv_header_pair(http_handle, line, deli_idx, &line[deli_idx + 2], (uint32_t)(idx - deli_idx - 4));
							*head_type = 1;
						}
                    }else if ((deli_idx + 2 == strlen("Transfer-Encoding: ")) && (memcmp(line, "Transfer-Encoding: ", deli_idx + 2) == 0)){
                        LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
						*body_total_len = 0;
						*head_type = 2;
					}
					else if ((deli_idx + 2 == strlen("Content-Range: ")) && (memcmp(line, "Content-Range: ", deli_idx + 2) == 0)) {
						int i = 0;
                        LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
						for (i = 0; i < idx; i++) {
							if (line[i] == '/') {
								break;
							}
						}
						_core_http_recv_header_pair(http_handle, line, deli_idx, &line[i + 1], (uint32_t)(idx - i - 3));
					}
                    
                }
            }
        }
        idx = 0;
        memset(line, 0, line_max_len);
    }
    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    http_handle->sysdep->core_sysdep_free(line);
    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    return res;
}

static int32_t _core_http_recv_body(core_http_handle_t *http_handle, uint32_t body_total_len, uint32_t head_type)
{
    int32_t res = STATE_SUCCESS;
    char *buffer = NULL;
    uint32_t remaining_len = 0, buffer_len = 0;

	if (head_type == 0) {
		//未获取到头类型，表明已经读取完
		return STATE_HTTP_READ_BODY_FINISHED;
	}

	if (head_type == 1) {
		if (http_handle->session.body_total_len == 0 && body_total_len == 0) {
			return STATE_HTTP_READ_BODY_EMPTY;
		}

		//if chunked,get real body_length
		if (http_handle->session.body_total_len == CORE_HTTP_DEFAULT_CHUNKED_MAX_SIZE && body_total_len == CORE_HTTP_DEFAULT_CHUNKED_MAX_SIZE) {

		}

		if (body_total_len != 0) {
			http_handle->session.body_total_len = body_total_len;
			remaining_len = body_total_len;
		}
		else {
			remaining_len = http_handle->session.body_total_len - http_handle->session.body_read_len;
		}

    if (remaining_len == 0) {
        return STATE_HTTP_READ_BODY_FINISHED;
    }

    buffer_len = (remaining_len < http_handle->body_buffer_max_len) ? (remaining_len) : (http_handle->body_buffer_max_len);

    buffer = http_handle->sysdep->core_sysdep_malloc(buffer_len, CORE_HTTP_MODULE_NAME);
    if (buffer == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }
    memset(buffer, 0, buffer_len);

    http_handle->sysdep->core_sysdep_mutex_lock(http_handle->recv_mutex);
    res = _core_http_recv(http_handle, (uint8_t *)buffer, buffer_len, http_handle->recv_timeout_ms);
    http_handle->sysdep->core_sysdep_mutex_unlock(http_handle->recv_mutex);
    if (res > 0) {
        aiot_http_recv_t packet;

        //core_log_hexdump(STATE_HTTP_LOG_RECV_CONTENT, '<', (uint8_t *)buffer, res);

        if (http_handle->core_recv_handler != NULL) {
            http_handle->session.body_read_len += res;
            memset(&packet, 0, sizeof(aiot_http_recv_t));
            packet.type = AIOT_HTTPRECV_BODY;
            packet.data.body.buffer = (uint8_t *)buffer;
            packet.data.body.len = res;

				http_handle->core_recv_handler(http_handle, &packet, http_handle->core_userdata);
			}
		}
		http_handle->sysdep->core_sysdep_free(buffer);
	}
	else if (head_type == 2) {
		uint32_t idx = 0;
		char *body = NULL;
		char *tempBody = NULL;
		uint32_t read_body_len = 0;
		char chunk[10] = {0};
		uint32_t chunk_size = 0;
		int body_end = 0;
		int chunk_end = 0;
        LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
		while (body_end == 0) {
			while (chunk_end == 0) {
				if (idx >= 10) {
                    
					//chunk size太长了
					return -1;
				}
				if ((res = _core_http_recv(http_handle, (uint8_t *)&chunk[idx], 1,
					http_handle->recv_timeout_ms)) < STATE_SUCCESS) {
					LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
					break;
				}
				idx++;
				if (idx >= 2 && chunk[idx - 1] == '\n' && chunk[idx - 2] == '\r') {
					core_hexStr2uint(chunk, (uint8_t)(idx - 2), &chunk_size);
					idx = 0;
                    LK_LOG("%s %d,chunk_size=%d\r\n",__FUNCTION__,__LINE__,chunk_size);
                    lk_msleep(10);
					memset(chunk, 0, 10);
					break;
				}
			}
			if (chunk_size > 0) {
                LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
				if (read_body_len == 0) {
					body = http_handle->sysdep->core_sysdep_malloc(chunk_size +1, CORE_HTTP_MODULE_NAME);
					if (body == NULL) {
						return STATE_SYS_DEPEND_MALLOC_FAILED;
					}
					memset(body, 0, chunk_size + 1);
					//http_handle->sysdep->core_sysdep_mutex_lock(http_handle->recv_mutex);
					res = _core_http_recv(http_handle, (uint8_t *)body, chunk_size, http_handle->recv_timeout_ms);
                    if(res != chunk_size)
                    {
                        LK_LOG("%s %d,recv error res=%d\r\n",__FUNCTION__,__LINE__,res);
                        return -1;
                    }
					//http_handle->sysdep->core_sysdep_mutex_unlock(http_handle->recv_mutex);
					read_body_len = chunk_size;

					char tail[10] = { 0 };
					if ((res = _core_http_recv(http_handle, (uint8_t *)&tail[0], 2,
						http_handle->recv_timeout_ms)) < STATE_SUCCESS) {
						//invalid chunk
						LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
						return -1;
					} 
					if (tail[0] == '\r' && tail[1] == '\n') {
                        LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
                        lk_msleep(10);
						continue;
					}
					else {
                        LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
						//invalid chunk
						return -1;
					}
                    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
					
				} else {
				    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
					tempBody = http_handle->sysdep->core_sysdep_malloc(read_body_len + 1, CORE_HTTP_MODULE_NAME);
					if (tempBody == NULL) {
						return STATE_SYS_DEPEND_MALLOC_FAILED;
					}
					memset(tempBody, 0, read_body_len + 1);
					memcpy(tempBody, body, read_body_len);
					http_handle->sysdep->core_sysdep_free(body);
					body = NULL;
					body = http_handle->sysdep->core_sysdep_malloc(read_body_len + chunk_size + 1, CORE_HTTP_MODULE_NAME);
					if (body == NULL) {
                        LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
						http_handle->sysdep->core_sysdep_free(tempBody);
						return STATE_SYS_DEPEND_MALLOC_FAILED;
					}
					memset(body, 0, read_body_len + chunk_size + 1);
					memcpy(body, tempBody, read_body_len);
					http_handle->sysdep->core_sysdep_free(tempBody);
                    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
					//http_handle->sysdep->core_sysdep_mutex_lock(http_handle->recv_mutex);
					res = _core_http_recv(http_handle, (uint8_t *)&body[read_body_len], chunk_size, http_handle->recv_timeout_ms);
					//http_handle->sysdep->core_sysdep_mutex_unlock(http_handle->recv_mutex);

					read_body_len += chunk_size;
					char tail[10] = { 0 };
					if ((res = _core_http_recv(http_handle, (uint8_t *)&tail[0], 2,
						http_handle->recv_timeout_ms)) < STATE_SUCCESS) {
						//invalid chunk
						LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
						return -1;
					}
					if (tail[0] == '\r' && tail[1] == '\n') {
                        
						continue;
					}
					else {
                        LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
						//invalid chunk
						return -1;
					}
				}
			} else {
				//chunked读取内容之后，要再把最后一个\r\n读取完，防止连续请求时读取header出错
				LK_LOG("%s %d idx=%d\r\n",__FUNCTION__,__LINE__,idx);
				_core_http_recv(http_handle, (uint8_t *)&chunk[idx], 2,
					http_handle->recv_timeout_ms);
				body_end = 1;
                lk_msleep(10);
			}
            LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);

		}
        #if 1
		if (read_body_len > 0) {
			char value[10] = { 0 };
			uint8_t output_len = 0;
            LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
			core_int2str(read_body_len, value, &output_len);
			//借用这个回调，给content_total_len赋值，读完body之后会用content_total_len做拷贝，返回给调用者
			_core_http_recv_header_pair(http_handle, "Content-Length", strlen("Content-Length"), value, (uint32_t)output_len);
			aiot_http_recv_t packet;
			core_log_hexdump(STATE_HTTP_LOG_RECV_CONTENT, '<', (uint8_t *)body, read_body_len);
            lk_msleep(50);
			if (http_handle->core_recv_handler != NULL) {
                LK_LOG("%s %d session.body_read_len=%d\r\n",__FUNCTION__,__LINE__,http_handle->session.body_read_len);
				http_handle->session.body_read_len += read_body_len;
                LK_LOG("%s %d session.body_read_len=%d\r\n",__FUNCTION__,__LINE__,http_handle->session.body_read_len);
				memset(&packet, 0, sizeof(aiot_http_recv_t));
				packet.type = AIOT_HTTPRECV_BODY;
				packet.data.body.buffer = (uint8_t *)body;
				packet.data.body.len = read_body_len;
                LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
                lk_msleep(50);
				http_handle->core_recv_handler(http_handle, &packet, http_handle->core_userdata);
			}
			
            LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
			http_handle->sysdep->core_sysdep_free(body);
		}
        res =  STATE_HTTP_READ_BODY_FINISHED;
        #endif
	}



    return res;
}

int32_t core_http_recv(void *handle)
{
    int32_t res = STATE_SUCCESS;
    uint32_t body_total_len = 0;
	uint32_t head_type = 0; //1 有Content-Length 2无Content-Lenth,Transfer-Encoding为chunked
    core_http_handle_t *http_handle = (core_http_handle_t *)handle;

    if (http_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (http_handle->network_handle == NULL) {
        return STATE_SYS_DEPEND_NWK_CLOSED;
    }

    if (http_handle->core_exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _core_http_exec_inc(http_handle);

    if (http_handle->session.sm == CORE_HTTP_SM_READ_HEADER) {
        http_handle->sysdep->core_sysdep_mutex_lock(http_handle->recv_mutex);
        res = _core_http_recv_header(http_handle, &body_total_len, &head_type);
        http_handle->sysdep->core_sysdep_mutex_unlock(http_handle->recv_mutex);
        if (res < STATE_SUCCESS) {
            _core_http_exec_dec(http_handle);
            return res;
        }
    }
    http_handle->session.sm = CORE_HTTP_SM_READ_BODY;

    res = _core_http_recv_body(http_handle, body_total_len, head_type);
    if (res == STATE_HTTP_READ_BODY_FINISHED || res == STATE_HTTP_READ_BODY_EMPTY) {
        memset(&http_handle->session, 0, sizeof(core_http_session_t));
    }

    _core_http_exec_dec(http_handle);

    return res;
}

int32_t core_http_deinit(void **p_handle)
{
    uint32_t deinit_timeout_ms = 0;
    core_http_handle_t *http_handle = NULL;
    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    if (p_handle == NULL || *p_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }
    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    http_handle = *(core_http_handle_t **)p_handle;

    if (http_handle->core_exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }
    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    http_handle->exec_enabled = 0;
    deinit_timeout_ms = http_handle->deinit_timeout_ms;
    do {
        if (http_handle->exec_count == 0) {
            break;
        }
        http_handle->sysdep->core_sysdep_sleep(CORE_HTTP_DEINIT_INTERVAL_MS);
    } while ((deinit_timeout_ms > CORE_HTTP_DEINIT_INTERVAL_MS) && (deinit_timeout_ms - CORE_HTTP_DEINIT_INTERVAL_MS > 0));

    if (http_handle->exec_count != 0) {
        return STATE_HTTP_DEINIT_TIMEOUT;
    }
    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    lk_msleep(100);
    if (http_handle->network_handle != NULL) {
        http_handle->sysdep->core_sysdep_network_deinit(&http_handle->network_handle);
    }
    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    lk_msleep(100);
    if (http_handle->host != NULL) {
        http_handle->sysdep->core_sysdep_free(http_handle->host);
    }
    if (http_handle->cred != NULL) {
        http_handle->sysdep->core_sysdep_free(http_handle->cred);
    }
    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    lk_msleep(100);
    http_handle->sysdep->core_sysdep_mutex_deinit(&http_handle->data_mutex);
    http_handle->sysdep->core_sysdep_mutex_deinit(&http_handle->send_mutex);
    http_handle->sysdep->core_sysdep_mutex_deinit(&http_handle->recv_mutex);
    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    http_handle->sysdep->core_sysdep_free(http_handle);
    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    *p_handle = NULL;
    lk_msleep(100);
    return STATE_SUCCESS;
}

//const char *dump_d = "485454502f312e3120323030204f4b0d0a5365727665723a206e67696e780d0a446174653a205468752c2031372053657020323032302031313a34353a343220474d540d0a436f6e74656e742d547970653a206170706c69636174696f6e2f7868746d6c2b786d6c0d0a5472616e736665722d456e636f64696e673a206368756e6b65640d0a436f6e6e656374696f6e3a206b6565702d616c6976650d0a566172793a204f726967696e0d0a566172793a204163636573732d436f6e74726f6c2d526571756573742d4d6574686f640d0a566172793a204163636573732d436f6e74726f6c2d526571756573742d486561646572730d0a0d0a34620d0a3c523e0a20203c6d73673ee69caae79fa5e5bc82e5b8b8efbc8ce8afb7e88194e7b3bbe7aea1e79086e591983c2f6d73673e0a20203c636f64653e3530303c2f636f64653e0a3c2f523e0a0d0a300d0a0d0a";
const char *dump_d = "485454502f312e3120323030204f4b0d0a5365727665723a206e67696e780d0a446174653a204672692c2031382053657020323032302030333a33373a343620474d540d0a436f6e74656e742d547970653a206170706c69636174696f6e2f6a736f6e0d0a5472616e736665722d456e636f64696e673a206368756e6b65640d0a436f6e6e656374696f6e3a206b6565702d616c6976650d0a566172793a204f726967696e0d0a566172793a204163636573732d436f6e74726f6c2d526571756573742d4d6574686f640d0a566172793a204163636573732d436f6e74726f6c2d526571756573742d486561646572730d0a0d0a32390d0a7b226d7367223a22666f7461207265636569766564207375636365737321222c22636f6465223a307d0d0a300d0a0d0a";
uint8_t hex_d[512] = {0};
int hex_offset = 0;
static int32_t _core_http_recv_debug(core_http_handle_t *http_handle, uint8_t *buffer, uint32_t len, uint32_t timeout_ms)
{
    LK_LOG("%s %d len=%d offset=%d\r\n",__FUNCTION__,__LINE__,len,hex_offset);
    memcpy(buffer,hex_d+hex_offset,len);
    hex_offset += len;
    
    return len;
}

static int32_t _core_http_recv_header_debug(core_http_handle_t *http_handle, uint32_t *body_total_len, uint32_t *head_type)
{
    int32_t res = STATE_SUCCESS;
    char *line = NULL;
    uint32_t idx = 0, line_max_len = http_handle->header_line_max_len;
    uint64_t timenow_ms = 0;
    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    line = http_handle->sysdep->core_sysdep_malloc(line_max_len, CORE_HTTP_MODULE_NAME);
    if (line == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }
    memset(line, 0, line_max_len);
    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    timenow_ms = http_handle->sysdep->core_sysdep_time();
    for (idx = 0; idx < line_max_len;) {
        if (timenow_ms > http_handle->sysdep->core_sysdep_time()) {
            timenow_ms = http_handle->sysdep->core_sysdep_time();
        }
        if (http_handle->sysdep->core_sysdep_time() - timenow_ms >= http_handle->recv_timeout_ms) {
            res =  STATE_HTTP_HEADER_INVALID;
            break;
        }
        if (idx + 2 > line_max_len) {
            res = STATE_HTTP_HEADER_BUFFER_TOO_SHORT;
            break;
        }
        /* read http header, "\r\n" in the end */
        if ((res = _core_http_recv_debug(http_handle, (uint8_t *)&line[idx], 1,
                                   http_handle->recv_timeout_ms)) < STATE_SUCCESS) {
            break;
        }
        idx++;
        if (res == 0 || (line[idx - 1] != '\r')) {
            continue;
        }
        if ((res = _core_http_recv_debug(http_handle, (uint8_t *)&line[idx], 1,
                                   http_handle->recv_timeout_ms)) < STATE_SUCCESS) {
            break;
        }
        idx++;
        if (res == 0 || (line[idx - 1] != '\n')) {
            continue;
        }
        LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
        core_log2(http_handle->sysdep, STATE_HTTP_LOG_RECV_HEADER, "< %.*s", &idx, line);
        LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
        /* next line should be http response body */
        if (idx == 2) {
            break;
        }
        LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
        /* status code */
        if ((idx > (strlen("HTTP/1.1 ") + 3)) && (memcmp(line, "HTTP/1.1 ", strlen("HTTP/1.1 "))) == 0) {
            uint32_t status_code = 0, code_idx = 0;
            for (code_idx = strlen("HTTP/1.1 "); code_idx < idx; code_idx++) {
                if (line[code_idx] < '0' || line[code_idx] > '9') {
                    break;
                }
            }
            LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
            res = core_str2uint(&line[strlen("HTTP/1.1 ")], (code_idx - strlen("HTTP/1.1 ")), &status_code);
            if (res < STATE_SUCCESS) {
                res = STATE_HTTP_STATUS_LINE_INVALID;
                break;
            }
            LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
            //_core_http_recv_status_code(http_handle, status_code);
        }
        /* header */
        {
            LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
            uint32_t deli_idx = 0;
            for (deli_idx = 0; deli_idx < idx; deli_idx++) {
                if (line[deli_idx] == ':' && line[deli_idx + 1] == ' ') {
                    if ((deli_idx + 2 == strlen("Content-Length: ")) && (memcmp(line, "Content-Length: ", deli_idx + 2) == 0)) {
						//如果同时存在Transfer-Encoding和Content-Length，忽略Content-Length
						if (*head_type == 0) {
							core_str2uint(&line[deli_idx + 2], (uint32_t)(idx - deli_idx - 4), body_total_len); //十进制的长度转换
							_core_http_recv_header_pair(http_handle, line, deli_idx, &line[deli_idx + 2], (uint32_t)(idx - deli_idx - 4));
							*head_type = 1;
						}
                    }else if ((deli_idx + 2 == strlen("Transfer-Encoding: ")) && (memcmp(line, "Transfer-Encoding: ", deli_idx + 2) == 0)){
                        LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
						*body_total_len = 0;
						*head_type = 2;
					}
					else if ((deli_idx + 2 == strlen("Content-Range: ")) && (memcmp(line, "Content-Range: ", deli_idx + 2) == 0)) {
						int i = 0;
						for (i = 0; i < idx; i++) {
							if (line[i] == '/') {
								break;
							}
						}
						_core_http_recv_header_pair(http_handle, line, deli_idx, &line[i + 1], (uint32_t)(idx - i - 3));
					}
                    
                }
            }
        }
        idx = 0;
        memset(line, 0, line_max_len);
    }
    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    http_handle->sysdep->core_sysdep_free(line);
    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    return res;
}

static int32_t _core_http_recv_body_debug(core_http_handle_t *http_handle, uint32_t body_total_len, uint32_t head_type)
{
    int32_t res = STATE_SUCCESS;
    char *buffer = NULL;
    uint32_t remaining_len = 0, buffer_len = 0;

	if (head_type == 0) {
		//未获取到头类型，表明已经读取完
		return STATE_HTTP_READ_BODY_FINISHED;
	}

	if (head_type == 1) {
		if (http_handle->session.body_total_len == 0 && body_total_len == 0) {
			return STATE_HTTP_READ_BODY_EMPTY;
		}

		//if chunked,get real body_length
		if (http_handle->session.body_total_len == CORE_HTTP_DEFAULT_CHUNKED_MAX_SIZE && body_total_len == CORE_HTTP_DEFAULT_CHUNKED_MAX_SIZE) {

		}

		if (body_total_len != 0) {
			http_handle->session.body_total_len = body_total_len;
			remaining_len = body_total_len;
		}
		else {
			remaining_len = http_handle->session.body_total_len - http_handle->session.body_read_len;
		}

    if (remaining_len == 0) {
        return STATE_HTTP_READ_BODY_FINISHED;
    }

    buffer_len = (remaining_len < http_handle->body_buffer_max_len) ? (remaining_len) : (http_handle->body_buffer_max_len);

    buffer = http_handle->sysdep->core_sysdep_malloc(buffer_len, CORE_HTTP_MODULE_NAME);
    if (buffer == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }
    memset(buffer, 0, buffer_len);

    http_handle->sysdep->core_sysdep_mutex_lock(http_handle->recv_mutex);
    res = _core_http_recv_debug(http_handle, (uint8_t *)buffer, buffer_len, http_handle->recv_timeout_ms);
    http_handle->sysdep->core_sysdep_mutex_unlock(http_handle->recv_mutex);
    if (res > 0) {
        aiot_http_recv_t packet;

        //core_log_hexdump(STATE_HTTP_LOG_RECV_CONTENT, '<', (uint8_t *)buffer, res);

        if (http_handle->core_recv_handler != NULL) {
            http_handle->session.body_read_len += res;
            memset(&packet, 0, sizeof(aiot_http_recv_t));
            packet.type = AIOT_HTTPRECV_BODY;
            packet.data.body.buffer = (uint8_t *)buffer;
            packet.data.body.len = res;

				http_handle->core_recv_handler(http_handle, &packet, http_handle->core_userdata);
			}
		}
		http_handle->sysdep->core_sysdep_free(buffer);
	}
	else if (head_type == 2) {
		uint32_t idx = 0;
		char *body = NULL;
		char *tempBody = NULL;
		uint32_t read_body_len = 0;
		char chunk[10] = {0};
		uint32_t chunk_size = 0;
		int body_end = 0;
		int chunk_end = 0;
        LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
		while (body_end == 0) {
			while (chunk_end == 0) {
				if (idx >= 10) {
					//chunk size太长了
					return -1;
				}
				if ((res = _core_http_recv_debug(http_handle, (uint8_t *)&chunk[idx], 1,
					http_handle->recv_timeout_ms)) < STATE_SUCCESS) {
					LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
					break;
				}
				idx++;
				if (idx >= 2 && chunk[idx - 1] == '\n' && chunk[idx - 2] == '\r') {
					core_hexStr2uint(&chunk[0], (uint32_t)(idx - 2), &chunk_size);
					idx = 0;
                    LK_LOG("%s %d,chunk_size=%d\r\n",__FUNCTION__,__LINE__,chunk_size);
					memset(chunk, 0, 10);
					break;
				}
			}
			if (chunk_size > 0) {
                LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
				if (read_body_len == 0) {
					body = http_handle->sysdep->core_sysdep_malloc(chunk_size +1, CORE_HTTP_MODULE_NAME);
					if (body == NULL) {
						return STATE_SYS_DEPEND_MALLOC_FAILED;
					}
					memset(body, 0, chunk_size + 1);
					http_handle->sysdep->core_sysdep_mutex_lock(http_handle->recv_mutex);
					res = _core_http_recv_debug(http_handle, (uint8_t *)body, chunk_size, http_handle->recv_timeout_ms);
					http_handle->sysdep->core_sysdep_mutex_unlock(http_handle->recv_mutex);
					read_body_len = chunk_size;

					char tail[10] = { 0 };
					if ((res = _core_http_recv_debug(http_handle, (uint8_t *)&tail[0], 2,
						http_handle->recv_timeout_ms)) < STATE_SUCCESS) {
						//invalid chunk
						LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
						return -1;
					} 
					if (tail[0] == '\r' && tail[1] == '\n') {
						continue;
					}
					else {
                        LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
						//invalid chunk
						return -1;
					}
                    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
					
				} else {
				    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
					tempBody = http_handle->sysdep->core_sysdep_malloc(read_body_len + 1, CORE_HTTP_MODULE_NAME);
					if (tempBody == NULL) {
						return STATE_SYS_DEPEND_MALLOC_FAILED;
					}
					memset(tempBody, 0, read_body_len + 1);
					memcpy(tempBody, body, read_body_len);
					http_handle->sysdep->core_sysdep_free(body);
					body = NULL;
					body = http_handle->sysdep->core_sysdep_malloc(read_body_len + chunk_size + 1, CORE_HTTP_MODULE_NAME);
					if (body == NULL) {
                        LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
						http_handle->sysdep->core_sysdep_free(tempBody);
						return STATE_SYS_DEPEND_MALLOC_FAILED;
					}
					memset(body, 0, read_body_len + chunk_size + 1);
					memcpy(body, tempBody, read_body_len);
					http_handle->sysdep->core_sysdep_free(tempBody);
                    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
					http_handle->sysdep->core_sysdep_mutex_lock(http_handle->recv_mutex);
					res = _core_http_recv_debug(http_handle, (uint8_t *)&body[read_body_len], chunk_size, http_handle->recv_timeout_ms);
					http_handle->sysdep->core_sysdep_mutex_unlock(http_handle->recv_mutex);

					read_body_len += chunk_size;
					char tail[10] = { 0 };
					if ((res = _core_http_recv_debug(http_handle, (uint8_t *)&tail[0], 2,
						http_handle->recv_timeout_ms)) < STATE_SUCCESS) {
						//invalid chunk
						LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
						return -1;
					}
					if (tail[0] == '\r' && tail[1] == '\n') {
                        
						continue;
					}
					else {
                        LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
						//invalid chunk
						return -1;
					}
				}
			} else {
				//chunked读取内容之后，要再把最后一个\r\n读取完，防止连续请求时读取header出错
				_core_http_recv_debug(http_handle, (uint8_t *)&chunk[idx], 2,
					http_handle->recv_timeout_ms);
				body_end = 1;
			}
            LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);

		}
		if (read_body_len > 0) {
			char value[10] = { 0 };
			uint8_t output_len = 0;
            LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
			core_int2str(read_body_len, value, &output_len);
			//借用这个回调，给content_total_len赋值，读完body之后会用content_total_len做拷贝，返回给调用者
			_core_http_recv_header_pair(http_handle, "Content-Length", strlen("Content-Length"), value, (uint32_t)output_len);
			aiot_http_recv_t packet;
			core_log_hexdump(STATE_HTTP_LOG_RECV_CONTENT, '<', (uint8_t *)body, read_body_len);
			if (http_handle->core_recv_handler != NULL) {
				http_handle->session.body_read_len += read_body_len;
				memset(&packet, 0, sizeof(aiot_http_recv_t));
				packet.type = AIOT_HTTPRECV_BODY;
				packet.data.body.buffer = (uint8_t *)body;
				packet.data.body.len = read_body_len;
                LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
				http_handle->core_recv_handler(http_handle, &packet, http_handle->core_userdata);
			}
            LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
			http_handle->sysdep->core_sysdep_free(body);
		}
	}



    return res;
}
#if 0
#include "cJSON.h"
#include "ty_os_adpt.h"

extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;
extern int32_t demo_state_logcb1(int32_t code, char *message);
void http_chunk_test()
{
    hex_offset = 0;
    asc2hex (hex_d, dump_d, strlen(dump_d));

	/* ????SDK???????? */
	aiot_sysdep_set_portfile(&g_aiot_sysdep_portfile);
	/* ????SDK???????? */
	aiot_state_set_logcb(demo_state_logcb1);

    core_http_handle_t *http_handle = core_http_init ();
    uint32_t body_total_len  = 0;
    uint32_t  head_type;
    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    _core_http_recv_header_debug (http_handle, &body_total_len, &head_type);
    LK_LOG("%s %d,head_type=%d\r\n",__FUNCTION__,__LINE__,head_type);
    
    _core_http_recv_body_debug (http_handle,body_total_len, head_type);

    LK_LOG("%s %d,body_total_len=%d\r\n",__FUNCTION__,__LINE__,body_total_len);

    core_http_deinit (&http_handle);

    LK_LOG("%s %d\r\n",__FUNCTION__,__LINE__);
}


int ty_FotaUploadErrMsg_test(const char *url,const char *task_no,int errcode)
{
    cJSON *root = NULL;
    char host[128] = {0};
    char uri[128] = {0};
    char imei[20] = {0};
    int ret;
    char *jsonStr = NULL;

    LK_LOG("%s Enter url=%s\r\n",__FUNCTION__,url);
    
    ret = httpc_parser_url (url,host, sizeof(host),  uri,sizeof(uri));
    if(ret != 0)
    {
        LK_LOG("%s httpc_parser_url error,ret=%d\r\n",__FUNCTION__,ret);
        return ret;
    }
    root =  cJSON_CreateObject();
    if(!root)
    {
        return -1;
    }
    if (task_no)
    {
        cJSON_AddItemToObject(root, "task_no", cJSON_CreateString(task_no));
    }
    at_imei(imei);
    cJSON_AddItemToObject(root, "imei", cJSON_CreateString(imei));
    cJSON_AddItemToObject(root, "err_code", cJSON_CreateNumber(errcode));
    jsonStr = cJSON_PrintUnformatted(root);
    int code = -1;
    int i ;
    http_chunk_test ();
    /*
    for( i = 0;i < 3;i++)
    {
        char *resp = http_post_util (host, uri, jsonStr,ty_http_ca_cert,HTTP_Content_Type_JSON,&code);
        if(resp)
        {
            LK_LOG("%s resp=%s\r\n",resp);
            lpa_free(resp);
            resp = NULL;
        }
        if(code == 0)
        {
            LK_LOG("%s http_post_util success \r\n",__FUNCTION__);
            break;
        }
    }
    */
    if(root)
        cJSON_Delete(root);
    
    if(jsonStr)
        lpa_free(jsonStr);
    LK_LOG("%s Leave \r\n",__FUNCTION__);
    return 0;
}

#endif
