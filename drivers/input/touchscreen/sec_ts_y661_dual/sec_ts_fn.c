/* drivers/input/touchscreen/sec_ts_fn.c
 *
 * Copyright (C) 2015 Samsung Electronics Co., Ltd.
 * http://www.samsungsemi.com/
 *
 * Core file for Samsung TSC driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "sec_ts.h"
#include "sec_ts_fac_spec.h"

static void fw_update(void *device_data);
static void get_fw_ver_bin(void *device_data);
static void get_fw_ver_ic(void *device_data);
static void get_config_ver(void *device_data);
static void get_cm_spec_over(void *device_data);
static void get_boca_type(void *device_data);
static void get_threshold(void *device_data);
static void module_off_master(void *device_data);
static void module_on_master(void *device_data);
static void get_chip_vendor(void *device_data);
static void get_chip_name(void *device_data);
static void set_mis_cal_spec(void *device_data);
static void get_mis_cal_info(void *device_data);
static void get_wet_mode(void *device_data);
static void get_x_num(void *device_data);
static void get_y_num(void *device_data);
static void get_x_cross_routing(void *device_data);
static void get_y_cross_routing(void *device_data);
static void get_checksum_data(void *device_data);
static void run_reference_read(void *device_data);
static void run_reference_read_all(void *device_data);
static void get_reference(void *device_data);
static void run_rawcap_read(void *device_data);
static void run_rawcap_read_all(void *device_data);
static void get_rawcap(void *device_data);
static void run_delta_read(void *device_data);
static void run_delta_read_all(void *device_data);
static void get_delta(void *device_data);
static void run_raw_p2p_read(void *device_data);
static void run_raw_p2p_min_read_all(void *device_data);
static void run_raw_p2p_max_read_all(void *device_data);
static void run_self_reference_read(void *device_data);
static void run_self_reference_read_all(void *device_data);
static void run_self_rawcap_read(void *device_data);
static void run_self_rawcap_read_all(void *device_data);
static void run_self_delta_read(void *device_data);
static void run_self_delta_read_all(void *device_data);
static void run_self_raw_p2p_min_read_all(void *device_data);
static void run_self_raw_p2p_max_read_all(void *device_data);
static void run_rawdata_read_all(void *device_data);
static void run_force_calibration(void *device_data);
static void get_force_calibration(void *device_data);
static void get_gap_data_x_all(void *device_data);
static void get_gap_data_y_all(void *device_data);
#ifdef USE_PRESSURE_SENSOR
static void run_force_pressure_calibration(void *device_data);
static void set_pressure_test_mode(void *device_data);
static void run_pressure_filtered_strength_read_all(void *device_data);
static void run_pressure_strength_read_all(void *device_data);
static void run_pressure_rawdata_read_all(void *device_data);
static void run_pressure_offset_read_all(void *device_data);
static void set_pressure_strength(void *device_data);
static void set_pressure_rawdata(void *device_data);
static void set_pressure_data_index(void *device_data);
static void get_pressure_strength(void *device_data);
static void get_pressure_rawdata(void *device_data);
static void get_pressure_data_index(void *device_data);
static void set_pressure_strength_clear(void *device_data);
static void get_pressure_threshold(void *device_data);
static void set_pressure_user_level(void *device_data);
static void get_pressure_user_level(void *device_data);
#endif
static void run_trx_short_test(void *device_data);
#ifdef TCLM_CONCEPT
static void set_external_factory(void *device_data);
static void get_pat_information(void *device_data);
#endif
static void set_tsp_test_result(void *device_data);
static void get_tsp_test_result(void *device_data);
static void clear_tsp_test_result(void *device_data);
static void run_lowpower_selftest(void *device_data);
static void increase_disassemble_count(void *device_data);
static void get_disassemble_count(void *device_data);
static void glove_mode(void *device_data);
static void clear_cover_mode(void *device_data);
static void dead_zone_enable(void *device_data);
static void drawing_test_enable(void *device_data);
static void set_lowpower_mode(void *device_data);
static void set_wirelesscharger_mode(void *device_data);
static void spay_enable(void *device_data);
static void set_aod_rect(void *device_data);
static void get_aod_rect(void *device_data);
static void aod_enable(void *device_data);
static void set_grip_data(void *device_data);
static void dex_enable(void *device_data);
static void brush_enable(void *device_data);
static void set_touchable_area(void *device_data);
static void set_log_level(void *device_data);
static void debug(void *device_data);
static void factory_cmd_result_all(void *device_data);
static void check_connection(void *device_data);
static void not_support_cmd(void *device_data);

static int execute_selftest(struct sec_ts_data *ts, bool save_result);

#ifdef CONFIG_TRUSTONIC_TRUSTED_UI
#include <linux/t-base-tui.h>
extern int tui_force_close(uint32_t arg);
extern void tui_cover_mode_set(bool arg);
#endif

static struct sec_cmd sec_cmds[] = {
	{SEC_CMD("fw_update", fw_update),},
	{SEC_CMD("get_fw_ver_bin", get_fw_ver_bin),},
	{SEC_CMD("get_fw_ver_ic", get_fw_ver_ic),},
	{SEC_CMD("get_config_ver", get_config_ver),},
	{SEC_CMD("get_cm_spec_over", get_cm_spec_over),},
	{SEC_CMD("get_boca_type", get_boca_type),},
	{SEC_CMD("get_threshold", get_threshold),},
	{SEC_CMD("module_off_master", module_off_master),},
	{SEC_CMD("module_on_master", module_on_master),},
	{SEC_CMD("get_chip_vendor", get_chip_vendor),},
	{SEC_CMD("get_chip_name", get_chip_name),},
	{SEC_CMD("set_mis_cal_spec", set_mis_cal_spec),},
	{SEC_CMD("get_mis_cal_info", get_mis_cal_info),},
	{SEC_CMD("get_wet_mode", get_wet_mode),},
	{SEC_CMD("get_x_num", get_x_num),},
	{SEC_CMD("get_y_num", get_y_num),},
	{SEC_CMD("get_x_cross_routing", get_x_cross_routing),},
	{SEC_CMD("get_y_cross_routing", get_y_cross_routing),},
	{SEC_CMD("get_checksum_data", get_checksum_data),},
	{SEC_CMD("run_reference_read", run_reference_read),},
	{SEC_CMD("run_reference_read_all", run_reference_read_all),},
	{SEC_CMD("get_reference", get_reference),},
	{SEC_CMD("run_rawcap_read", run_rawcap_read),},
	{SEC_CMD("run_rawcap_read_all", run_rawcap_read_all),},
	{SEC_CMD("get_rawcap", get_rawcap),},
	{SEC_CMD("run_delta_read", run_delta_read),},
	{SEC_CMD("run_delta_read_all", run_delta_read_all),},
	{SEC_CMD("get_delta", get_delta),},
	{SEC_CMD("run_raw_p2p_read", run_raw_p2p_read),},
	{SEC_CMD("run_raw_p2p_min_read_all", run_raw_p2p_min_read_all),},
	{SEC_CMD("run_raw_p2p_max_read_all", run_raw_p2p_max_read_all),},
	{SEC_CMD("run_self_reference_read", run_self_reference_read),},
	{SEC_CMD("run_self_reference_read_all", run_self_reference_read_all),},
	{SEC_CMD("run_self_rawcap_read", run_self_rawcap_read),},
	{SEC_CMD("run_self_rawcap_read_all", run_self_rawcap_read_all),},
	{SEC_CMD("run_self_delta_read", run_self_delta_read),},
	{SEC_CMD("run_self_delta_read_all", run_self_delta_read_all),},
	{SEC_CMD("run_self_raw_p2p_min_read_all", run_self_raw_p2p_min_read_all),},
	{SEC_CMD("run_self_raw_p2p_max_read_all", run_self_raw_p2p_max_read_all),},
	{SEC_CMD("run_rawdata_read_all_for_ghost", run_rawdata_read_all),},
	{SEC_CMD("run_force_calibration", run_force_calibration),},
	{SEC_CMD("get_force_calibration", get_force_calibration),},
	{SEC_CMD("get_gap_data_x_all", get_gap_data_x_all),},
	{SEC_CMD("get_gap_data_y_all", get_gap_data_y_all),},
#ifdef USE_PRESSURE_SENSOR
	{SEC_CMD("run_force_pressure_calibration", run_force_pressure_calibration),},
	{SEC_CMD("set_pressure_test_mode", set_pressure_test_mode),},
	{SEC_CMD("run_pressure_filtered_strength_read_all", run_pressure_filtered_strength_read_all),},
	{SEC_CMD("run_pressure_strength_read_all", run_pressure_strength_read_all),},
	{SEC_CMD("run_pressure_rawdata_read_all", run_pressure_rawdata_read_all),},
	{SEC_CMD("run_pressure_offset_read_all", run_pressure_offset_read_all),},
	{SEC_CMD("set_pressure_strength", set_pressure_strength),},
	{SEC_CMD("set_pressure_rawdata", set_pressure_rawdata),},
	{SEC_CMD("set_pressure_data_index", set_pressure_data_index),},
	{SEC_CMD("get_pressure_strength", get_pressure_strength),},
	{SEC_CMD("get_pressure_rawdata", get_pressure_rawdata),},
	{SEC_CMD("get_pressure_data_index", get_pressure_data_index),},
	{SEC_CMD("set_pressure_strength_clear", set_pressure_strength_clear),},
	{SEC_CMD("get_pressure_threshold", get_pressure_threshold),},
	{SEC_CMD("set_pressure_user_level", set_pressure_user_level),},
	{SEC_CMD("get_pressure_user_level", get_pressure_user_level),},
#endif
	{SEC_CMD("run_trx_short_test", run_trx_short_test),},
#ifdef TCLM_CONCEPT
	{SEC_CMD("get_pat_information", get_pat_information),},
	{SEC_CMD("set_external_factory", set_external_factory),},
#endif
	{SEC_CMD("set_tsp_test_result", set_tsp_test_result),},
	{SEC_CMD("get_tsp_test_result", get_tsp_test_result),},
	{SEC_CMD("clear_tsp_test_result", clear_tsp_test_result),},
	{SEC_CMD("run_lowpower_selftest", run_lowpower_selftest),},
	{SEC_CMD("increase_disassemble_count", increase_disassemble_count),},
	{SEC_CMD("get_disassemble_count", get_disassemble_count),},
	{SEC_CMD("glove_mode", glove_mode),},
	{SEC_CMD("clear_cover_mode", clear_cover_mode),},
	{SEC_CMD("dead_zone_enable", dead_zone_enable),},
	{SEC_CMD("drawing_test_enable", drawing_test_enable),},
	{SEC_CMD("set_lowpower_mode", set_lowpower_mode),},
	{SEC_CMD("set_wirelesscharger_mode", set_wirelesscharger_mode),},
	{SEC_CMD("spay_enable", spay_enable),},
	{SEC_CMD("set_aod_rect", set_aod_rect),},
	{SEC_CMD("get_aod_rect", get_aod_rect),},
	{SEC_CMD("aod_enable", aod_enable),},
	{SEC_CMD("set_grip_data", set_grip_data),},
	{SEC_CMD("dex_enable", dex_enable),},
	{SEC_CMD("brush_enable", brush_enable),},
	{SEC_CMD("set_touchable_area", set_touchable_area),},
	{SEC_CMD("set_log_level", set_log_level),},
	{SEC_CMD("debug", debug),},
	{SEC_CMD("factory_cmd_result_all", factory_cmd_result_all),},
	{SEC_CMD("check_connection", check_connection),},
	{SEC_CMD("not_support_cmd", not_support_cmd),},
};

static ssize_t scrub_position_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[256] = { 0 };

#ifdef CONFIG_SAMSUNG_PRODUCT_SHIP
	input_info(true, &ts->client->dev,
			"%s: scrub_id: %d\n", __func__, ts->scrub_id);
#else
	input_info(true, &ts->client->dev,
			"%s: scrub_id: %d, X:%d, Y:%d\n", __func__,
			ts->scrub_id, ts->scrub_x, ts->scrub_y);
#endif
	snprintf(buff, sizeof(buff), "%d %d %d", ts->scrub_id, ts->scrub_x, ts->scrub_y);

	ts->scrub_x = 0;
	ts->scrub_y = 0;

	return snprintf(buf, PAGE_SIZE, "%s", buff);
}

static DEVICE_ATTR(scrub_pos, S_IRUGO, scrub_position_show, NULL);

static ssize_t read_ito_check_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[256] = { 0 };

	input_info(true, &ts->client->dev, "%s: %02X%02X%02X%02X\n", __func__,
			ts->ito_test[0], ts->ito_test[1],
			ts->ito_test[2], ts->ito_test[3]);

	snprintf(buff, sizeof(buff), "%02X%02X%02X%02X",
			ts->ito_test[0], ts->ito_test[1],
			ts->ito_test[2], ts->ito_test[3]);

	return snprintf(buf, SEC_CMD_BUF_SIZE, "%s", buff);
}

static ssize_t read_raw_check_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	int ii, ret = 0;
	char *buffer = NULL;
	char temp[CMD_RESULT_WORD_LEN] = { 0 };

	buffer = vzalloc(ts->rx_count * ts->tx_count * 6);
	if (!buffer)
		return -ENOMEM;

	memset(buffer, 0x00, ts->rx_count * ts->tx_count * 6);

	for (ii = 0; ii < (ts->rx_count * ts->tx_count - 1); ii++) {
		snprintf(temp, CMD_RESULT_WORD_LEN, "%d ", ts->pFrame[ii]);
		strncat(buffer, temp, CMD_RESULT_WORD_LEN);

		memset(temp, 0x00, CMD_RESULT_WORD_LEN);
	}

	snprintf(temp, CMD_RESULT_WORD_LEN, "%d", ts->pFrame[ii]);
	strncat(buffer, temp, CMD_RESULT_WORD_LEN);

	ret = snprintf(buf, ts->rx_count * ts->tx_count * 6, buffer);
	vfree(buffer);

	return ret;
}

static ssize_t read_multi_count_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	input_info(true, &ts->client->dev, "%s: %d\n", __func__,
			ts->multi_count);

	return snprintf(buf, SEC_CMD_BUF_SIZE, "%d", ts->multi_count);
}

static ssize_t clear_multi_count_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	ts->multi_count = 0;

	input_info(true, &ts->client->dev, "%s: clear\n", __func__);

	return count;
}

static ssize_t read_wet_mode_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	input_info(true, &ts->client->dev, "%s: %d, %d\n", __func__,
			ts->wet_count, ts->dive_count);

	return snprintf(buf, SEC_CMD_BUF_SIZE, "%d", ts->wet_count);
}

static ssize_t clear_wet_mode_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	ts->wet_count = 0;
	ts->dive_count= 0;

	input_info(true, &ts->client->dev, "%s: clear\n", __func__);

	return count;
}

static ssize_t read_noise_mode_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	input_info(true, &ts->client->dev, "%s: %d\n", __func__, ts->noise_count);

	return snprintf(buf, SEC_CMD_BUF_SIZE, "%d", ts->noise_count);
}

static ssize_t clear_noise_mode_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	ts->noise_count = 0;

	input_info(true, &ts->client->dev, "%s: clear\n", __func__);

	return count;
}

static ssize_t read_comm_err_count_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	input_info(true, &ts->client->dev, "%s: %d\n", __func__,
			ts->multi_count);

	return snprintf(buf, SEC_CMD_BUF_SIZE, "%d", ts->comm_err_count);
}

static ssize_t clear_comm_err_count_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	ts->comm_err_count = 0;

	input_info(true, &ts->client->dev, "%s: clear\n", __func__);

	return count;
}

static ssize_t read_module_id_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[256] = { 0 };

	input_info(true, &ts->client->dev, "%s: %d\n", __func__,
			ts->multi_count);

	snprintf(buff, sizeof(buff), "SE%02X%02X%02X%02X%1c%01X%02X%02X",
		ts->plat_data->panel_revision, ts->plat_data->img_version_of_bin[2],
		ts->plat_data->img_version_of_bin[3], ts->nv,
		ts->tdata->tclm_string[ts->tdata->cal_position].s_name, ts->tdata->cal_count & 0xF,
		ts->pressure_cal_base, ts->pressure_cal_delta);


	return snprintf(buf, SEC_CMD_BUF_SIZE, "%s", buff);
}

static ssize_t read_vendor_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	unsigned char buffer[10] = { 0 };

	snprintf(buffer, 5, ts->plat_data->firmware_name + 8);

	return snprintf(buf, SEC_CMD_BUF_SIZE, "LSI_%s", buffer);
}

static ssize_t clear_checksum_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	ts->checksum_result = 0;

	input_info(true, &ts->client->dev, "%s: clear\n", __func__);

	return count;
}

static ssize_t read_checksum_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	input_info(true, &ts->client->dev, "%s: %d\n", __func__,
			ts->checksum_result);

	return snprintf(buf, SEC_CMD_BUF_SIZE, "%d", ts->checksum_result);
}

static ssize_t clear_holding_time_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	ts->time_longest = 0;

	input_info(true, &ts->client->dev, "%s: clear\n", __func__);

	return count;
}

static ssize_t read_holding_time_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	input_info(true, &ts->client->dev, "%s: %ld\n", __func__,
			ts->time_longest);

	return snprintf(buf, SEC_CMD_BUF_SIZE, "%ld", ts->time_longest);
}

static ssize_t read_all_touch_count_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	input_info(true, &ts->client->dev, "%s: touch:%d, force:%d, aod:%d, spay:%d\n", __func__,
			ts->all_finger_count, ts->all_force_count,
			ts->all_aod_tap_count, ts->all_spay_count);

	return snprintf(buf, SEC_CMD_BUF_SIZE,
			"\"TTCN\":\"%d\",\"TFCN\":\"%d\",\"TACN\":\"%d\",\"TSCN\":\"%d\"",
			ts->all_finger_count, ts->all_force_count,
			ts->all_aod_tap_count, ts->all_spay_count);
}

static ssize_t clear_all_touch_count_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	ts->all_force_count = 0;
	ts->all_aod_tap_count = 0;
	ts->all_spay_count = 0;

	input_info(true, &ts->client->dev, "%s: clear\n", __func__);

	return count;
}

static ssize_t read_z_value_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	input_info(true, &ts->client->dev, "%s: max:%d, min:%d, avg:%d\n", __func__,
			ts->max_z_value, ts->min_z_value,
			ts->sum_z_value);

	if (ts->all_finger_count)
		return snprintf(buf, SEC_CMD_BUF_SIZE,
				"\"TMXZ\":\"%d\",\"TMNZ\":\"%d\",\"TAVZ\":\"%d\"",
				ts->max_z_value, ts->min_z_value,
				ts->sum_z_value / ts->all_finger_count);
	else
		return snprintf(buf, SEC_CMD_BUF_SIZE,
				"\"TMXZ\":\"%d\",\"TMNZ\":\"%d\"",
				ts->max_z_value, ts->min_z_value);

}

static ssize_t clear_z_value_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	ts->max_z_value= 0;
	ts->min_z_value= 0xFFFFFFFF;
	ts->sum_z_value= 0;
	ts->all_finger_count = 0;

	input_info(true, &ts->client->dev, "%s: clear\n", __func__);

	return count;
}

static ssize_t sensitivity_mode_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char value_result[10] = { 0 };
	int ret;
	int value[5];

	ret = ts->sec_ts_i2c_read(ts, SEC_TS_READ_SENSITIVITY_VALUE, value_result, 10);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: i2c fail!, %d\n", __func__, ret);
		return ret;
	}

	value[0] = value_result[0]<<8 | value_result[1];
	value[1] = value_result[2]<<8 | value_result[3];
	value[2] = value_result[4]<<8 | value_result[5];
	value[3] = value_result[6]<<8 | value_result[7];
	value[4] = value_result[8]<<8 | value_result[9];

	input_info(true, &ts->client->dev, "%s: sensitivity mode,%d,%d,%d,%d,%d\n", __func__,
		value[0], value[1], value[2], value[3], value[4]);

	return snprintf(buf, SEC_CMD_BUF_SIZE, "%d,%d,%d,%d,%d",
			value[0], value[1], value[2], value[3], value[4]);

}

static ssize_t sensitivity_mode_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	int ret;
	u8 temp;
	unsigned long value = 0;

	if (count > 2)
		return -EINVAL;

	ret = kstrtoul(buf, 10, &value);
	if (ret != 0)
		return ret;

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: power off in IC\n", __func__);
		return 0;
	}

	input_err(true, &ts->client->dev, "%s: enable:%d\n", __func__, value);

	if (value == 1) {
		temp = 0x1;
		ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SENSITIVITY_MODE, &temp, 1);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: send sensitivity mode on fail!\n", __func__);
			return ret;
		}
		sec_ts_delay(30);
		input_info(true, &ts->client->dev, "%s: enable end\n", __func__);
	} else {
		temp = 0x0;
		ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SENSITIVITY_MODE, &temp, 1);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: send sensitivity mode off fail!\n", __func__);
			return ret;
		}
		input_info(true, &ts->client->dev, "%s: disable end\n", __func__);
	}

	input_info(true, &ts->client->dev, "%s: done\n", __func__);

	return count;
}

static ssize_t pressure_enable_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[256] = { 0 };

	input_info(true, &ts->client->dev, "%s: force touch %s\n",
			__func__, ts->lowpower_mode & SEC_TS_MODE_SPONGE_FORCE_KEY ? "ON" : "OFF");

	if (ts->lowpower_mode & SEC_TS_MODE_SPONGE_FORCE_KEY)
		snprintf(buff, sizeof(buff), "1");
	else
		snprintf(buff, sizeof(buff), "0");

	return snprintf(buf, SEC_CMD_BUF_SIZE, "%s\n", buff);
}

/* Factory & game tools	: OFF value [0] / ON value [1]	*/
/* Settings				: OFF value [2] / ON value [3]	*/
static ssize_t pressure_enable_strore(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	int ret;
	unsigned long value = 0;

	if (count > 2)
		return -EINVAL;

	ret = kstrtoul(buf, 10, &value);
	if (ret != 0)
		return ret;

	if (!ts->use_sponge)
		return -EINVAL;

	if (!ts->plat_data->support_pressure) {
		input_err(true, &ts->client->dev, "%s: Not support pressure[%d]\n",
				__func__, value);
		return -EINVAL;
	}

	input_info(true, &ts->client->dev, "%s: caller_id[%d]\n", __func__, value);

	if (value == 1 || value == 3 ) {
		ts->lowpower_mode |= SEC_TS_MODE_SPONGE_FORCE_KEY;
	} else if (value == 0 || value == 2 ) {
		ts->lowpower_mode &= ~SEC_TS_MODE_SPONGE_FORCE_KEY;
	} else {
		input_err(true, &ts->client->dev, "%s: Abnormal input value[%d]\n",
				__func__, value);
		return -EINVAL;
	}
	ts->pressure_caller_id = value;

	sec_ts_set_custom_library(ts);

	return count;
}

static ssize_t read_pressure_raw_check_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	/* need 22 char for each 'PRESSURE_CHANNEL_NUM * 4' data */
	char buff[PRESSURE_CHANNEL_NUM * 4 * 22] = {0};
	int data[4] = {TYPE_RAW_DATA, TYPE_SIGNAL_DATA,
		TYPE_REMV_AMB_DATA, TYPE_OFFSET_DATA_SEC};
	char loc[3] = {'R', 'C', 'L'};
	int i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < PRESSURE_CHANNEL_NUM; j++) {
			char tmp[20] = {0};
			snprintf(tmp, sizeof(tmp), "\"TP%02d%c\":\"%d\"",
					data[i], loc[j], ts->pressure_data[data[i]][j]);
			strncat(buff, tmp, sizeof(buff) - strlen(buff) - 1);
			if (i < 3 || j < PRESSURE_CHANNEL_NUM - 1)
				strncat(buff, ",", 2);
		}
	}
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);

	return snprintf(buf, SEC_CMD_BUF_SIZE, "%s", buff);
}

static ssize_t read_ambient_info_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	input_info(true, &ts->client->dev, "%s: max: %d(%d,%d), min: %d(%d,%d)\n", __func__,
			ts->max_ambient, ts->max_ambient_channel_tx, ts->max_ambient_channel_rx,
			ts->min_ambient, ts->min_ambient_channel_tx, ts->min_ambient_channel_rx);

	return snprintf(buf, SEC_CMD_BUF_SIZE,
			"\"TAMB_MAX\":\"%d\",\"TAMB_MAX_TX\":\"%d\",\"TAMB_MAX_RX\":\"%d\","
			"\"TAMB_MIN\":\"%d\",\"TAMB_MIN_TX\":\"%d\",\"TAMB_MIN_RX\":\"%d\"",
			ts->max_ambient, ts->max_ambient_channel_tx, ts->max_ambient_channel_rx,
			ts->min_ambient, ts->min_ambient_channel_tx, ts->min_ambient_channel_rx);
}

static ssize_t get_lp_dump(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	u8 string_data[8] = {0, };
	u16 current_index;
	int i, ret;

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: Touch is stopped!\n", __func__);
		return snprintf(buf, SEC_CMD_BUF_SIZE, "TSP turned off");
	}

	string_data[0] = SEC_TS_CMD_SPONGE_LP_DUMP & 0xFF;
	string_data[1] = (SEC_TS_CMD_SPONGE_LP_DUMP & 0xFF00) >> 8;

	disable_irq(ts->client->irq);

	ret = ts->sec_ts_read_sponge(ts, string_data, 2);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: Failed to read rect\n", __func__);
		snprintf(buf, SEC_CMD_BUF_SIZE, "NG, Failed to read rect");
		goto out;
	}

	current_index = (string_data[1] & 0xFF) << 8 | (string_data[0] & 0xFF);
	if (current_index > 1000 || current_index < 500) {
		input_err(true, &ts->client->dev,
				"Failed to Sponge LP log %d\n", current_index);
		snprintf(buf, SEC_CMD_BUF_SIZE,
				"NG, Failed to Sponge LP log, current_index=%d",
				current_index);
		goto out;
	}

	input_info(true, &ts->client->dev,
			"%s: DEBUG current_index = %d\n", __func__, current_index);

	/* sponge has 62 stacks for LP dump */
	for (i = 61; i >= 0; i--) {
		u16 data0, data1, data2, data3;
		char buff[30] = {0, };
		u16 string_addr;

		string_addr = current_index - (8 * i);
		if (string_addr < 500)
			string_addr += SEC_TS_CMD_SPONGE_LP_DUMP;
		string_data[0] = string_addr & 0xFF;
		string_data[1] = (string_addr & 0xFF00) >> 8;

		ret = ts->sec_ts_read_sponge(ts, string_data, 8);
		if (ret < 0) {
			input_err(true, &ts->client->dev,
					"%s: Failed to read rect\n", __func__);
			snprintf(buf, SEC_CMD_BUF_SIZE,
					"NG, Failed to read rect, addr=%d",
					string_addr);
			goto out;
		}

		data0 = (string_data[1] & 0xFF) << 8 | (string_data[0] & 0xFF);
		data1 = (string_data[3] & 0xFF) << 8 | (string_data[2] & 0xFF);
		data2 = (string_data[5] & 0xFF) << 8 | (string_data[4] & 0xFF);
		data3 = (string_data[7] & 0xFF) << 8 | (string_data[6] & 0xFF);
		if (data0 || data1 || data2 || data3) {
			snprintf(buff, sizeof(buff),
					"%d: %04x%04x%04x%04x\n",
					string_addr, data0, data1, data2, data3);
			strncat(buf, buff, sizeof(buf) - strlen(buf) - 1);
		}
	}

out:
	enable_irq(ts->client->irq);
	return strlen(buf);
}

static DEVICE_ATTR(ito_check, S_IRUGO, read_ito_check_show, NULL);
static DEVICE_ATTR(raw_check, S_IRUGO, read_raw_check_show, NULL);
static DEVICE_ATTR(multi_count, S_IRUGO | S_IWUSR | S_IWGRP, read_multi_count_show, clear_multi_count_store);
static DEVICE_ATTR(wet_mode, S_IRUGO | S_IWUSR | S_IWGRP, read_wet_mode_show, clear_wet_mode_store);
static DEVICE_ATTR(noise_mode, S_IRUGO | S_IWUSR | S_IWGRP, read_noise_mode_show, clear_noise_mode_store);
static DEVICE_ATTR(comm_err_count, S_IRUGO | S_IWUSR | S_IWGRP, read_comm_err_count_show, clear_comm_err_count_store);
static DEVICE_ATTR(checksum, S_IRUGO | S_IWUSR | S_IWGRP, read_checksum_show, clear_checksum_store);
static DEVICE_ATTR(holding_time, S_IRUGO | S_IWUSR | S_IWGRP, read_holding_time_show, clear_holding_time_store);
static DEVICE_ATTR(all_touch_count, S_IRUGO | S_IWUSR | S_IWGRP, read_all_touch_count_show, clear_all_touch_count_store);
static DEVICE_ATTR(z_value, S_IRUGO | S_IWUSR | S_IWGRP, read_z_value_show, clear_z_value_store);
static DEVICE_ATTR(module_id, S_IRUGO, read_module_id_show, NULL);
static DEVICE_ATTR(vendor, S_IRUGO, read_vendor_show, NULL);
static DEVICE_ATTR(pressure_enable, S_IRUGO | S_IWUSR | S_IWGRP, pressure_enable_show, pressure_enable_strore);
static DEVICE_ATTR(pressure_raw_check, S_IRUGO, read_pressure_raw_check_show, NULL);
static DEVICE_ATTR(read_ambient_info, S_IRUGO, read_ambient_info_show, NULL);
static DEVICE_ATTR(get_lp_dump, S_IRUGO, get_lp_dump, NULL);
static DEVICE_ATTR(sensitivity_mode, 0664, sensitivity_mode_show, sensitivity_mode_store);

static struct attribute *cmd_attributes[] = {
	&dev_attr_scrub_pos.attr,
	&dev_attr_ito_check.attr,
	&dev_attr_raw_check.attr,
	&dev_attr_multi_count.attr,
	&dev_attr_wet_mode.attr,
	&dev_attr_noise_mode.attr,
	&dev_attr_comm_err_count.attr,
	&dev_attr_checksum.attr,
	&dev_attr_holding_time.attr,
	&dev_attr_all_touch_count.attr,
	&dev_attr_z_value.attr,
	&dev_attr_module_id.attr,
	&dev_attr_vendor.attr,
	&dev_attr_pressure_enable.attr,
	&dev_attr_pressure_raw_check.attr,
	&dev_attr_read_ambient_info.attr,
	&dev_attr_get_lp_dump.attr,
	&dev_attr_sensitivity_mode.attr,
	NULL,
};

static struct attribute_group cmd_attr_group = {
	.attrs = cmd_attributes,
};

static int sec_ts_check_index(struct sec_ts_data *ts)
{
	struct sec_cmd_data *sec = &ts->sec;
	char buff[SEC_CMD_STR_LEN] = { 0 };
	int node;

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > ts->tx_count
			|| sec->cmd_param[1] < 0 || sec->cmd_param[1] > ts->rx_count) {

		snprintf(buff, sizeof(buff), "%s", "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		input_info(true, &ts->client->dev, "%s: parameter error: %u, %u\n",
				__func__, sec->cmd_param[0], sec->cmd_param[0]);
		node = -1;
		return node;
	}
	node = sec->cmd_param[1] * ts->tx_count + sec->cmd_param[0];
	input_info(true, &ts->client->dev, "%s: node = %d\n", __func__, node);
	return node;
}
static void fw_update(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[64] = { 0 };
	int retval = 0;

	sec_cmd_set_default_result(sec);
#if defined(CONFIG_SAMSUNG_PRODUCT_SHIP)
	if (sec->cmd_param[0] == 1) {
		input_err(true, &ts->client->dev, "%s: user_ship, skip\n", __func__);
		snprintf(buff, sizeof(buff), "OK");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_OK;
		return;
	}
#endif
	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		snprintf(buff, sizeof(buff), "%s", "TSP turned off");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		return;
	}

	retval = sec_ts_firmware_update_on_hidden_menu(ts, sec->cmd_param[0]);
	if (retval < 0) {
		snprintf(buff, sizeof(buff), "%s", "NA");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		input_err(true, &ts->client->dev, "%s: failed [%d]\n", __func__, retval);
	} else {
		snprintf(buff, sizeof(buff), "%s", "OK");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_OK;
		input_info(true, &ts->client->dev, "%s: success [%d]\n", __func__, retval);
	}
}

int sec_ts_fix_tmode(struct sec_ts_data *ts, u8 mode, u8 state)
{
	int ret;
	u8 onoff[1] = {STATE_MANAGE_OFF};
	u8 tBuff[2] = { mode, state };

	input_info(true, &ts->client->dev, "%s\n", __func__);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_STATEMANAGE_ON, onoff, 1);
	sec_ts_delay(20);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_CHG_SYSMODE, tBuff, sizeof(tBuff));
	sec_ts_delay(20);

	return ret;
}

int sec_ts_p2p_tmode(struct sec_ts_data *ts)
{
	int ret;
	u8 mode[2] = {0x0E, 0x00};
	char para = TO_SELFTEST_MODE;

	input_info(true, &ts->client->dev, "%s\n", __func__);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, &para, 1);
	sec_ts_delay(30);
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_CLEAR_EVENT_STACK, NULL, 0);
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_P2P_MODE, mode, sizeof(mode));
	sec_ts_delay(30);

	return ret;
}

static int execute_p2ptest(struct sec_ts_data *ts)
{
	int ret;
	u8 test[2] = {0x00, 0x64};
	u8 tBuff[10] = {0};
	int retry = 0;

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_P2P_TEST, test, sizeof(test));
	sec_ts_delay(1000);

	ret = -1;

	while (ts->sec_ts_i2c_read(ts, SEC_TS_READ_ONE_EVENT, tBuff, 6)) {
		input_info(true, &ts->client->dev, "tBuff is %d, %d, %d, %d, %d, %d\n", tBuff[0], tBuff[1], tBuff[2], tBuff[3], tBuff[4], tBuff[5]);
		if (((tBuff[0] >> 2) & 0xF) == TYPE_STATUS_EVENT_VENDOR_INFO) {
			if (tBuff[1] == SEC_TS_VENDOR_ACK_CMR_TEST_DONE) {
				ts->cm_raw_set_p2p_max = (tBuff[2] & 0xFF) << 8 | (tBuff[3] & 0xFF);
				ts->cm_raw_set_p2p_min = (tBuff[4] & 0xFF) << 8 | (tBuff[5] & 0xFF);
				//ts->cm_raw_set_p2p_diff = (tBuff[7] & 0xC0) << 2 | (tBuff[6] & 0xFF);
			}
		}

		if (gpio_get_value(ts->plat_data->irq_gpio)) {
			input_info(true, &ts->client->dev, "%s: left event is 0\n", __func__);
			ret = 0;
			break;
		}

		if (retry++ > SEC_TS_WAIT_RETRY_CNT) {
			input_err(true, &ts->client->dev, "%s: Time Over\n", __func__);
			break;
		}
		sec_ts_delay(20);
	}
	return ret;
}

int sec_ts_release_tmode(struct sec_ts_data *ts)
{
	int ret;
	u8 onoff[1] = {STATE_MANAGE_ON};

	input_info(true, &ts->client->dev, "%s\n", __func__);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_STATEMANAGE_ON, onoff, 1);
	sec_ts_delay(20);

	return ret;
}

static int sec_ts_get_boca_spec(struct sec_ts_data *ts)
{
	int lcdtype = 0;
	bool boca100 = 0;

#if defined(CONFIG_FB_MSM_MDSS_SAMSUNG)
#ifdef CONFIG_SEC_FACTORY
	lcdtype = get_lcd_attached("TSP");
#else
	lcdtype = get_lcd_attached("GET");
#endif
	if (lcdtype == 0xFFFFFF) {
		input_err(true, &ts->client->dev,
				"%s: lcd is not attached\n", __func__);
		return -ENODEV;
	}
#endif
#if defined(CONFIG_EXYNOS_DECON_FB)
	lcdtype = get_lcd_info("connected");
	if (lcdtype < 0) {
		input_err(true, &ts->client->dev,
				"%s: Failed to get lcd info\n", __func__);
		return -ENOENT;
	}

	if (!lcdtype) {
		input_err(true, &ts->client->dev,
				"%s: lcd is disconnected\n", __func__);
		return -ENODEV;
	}

	lcdtype = get_lcd_info("id");
	if (lcdtype < 0) {
		input_err(true, &ts->client->dev,
				"%s: Failed to get lcd info\n", __func__);
		return -ENOENT;
	}
#endif

	boca100 = ((lcdtype & 0x003000) >> 12) == 0x03;
	if (boca100) {
		cm_delta_xdir = cm_delta_xdir_100boca;
		cm_delta_ydir = cm_delta_ydir_100boca;
		pre_cm_delta_xdir = pre_cm_delta_xdir_100boca;
		pre_cm_delta_ydir = pre_cm_delta_ydir_100boca;
		cm_delta_avg_xdir = cm_delta_avg_xdir_100boca;
		cm_delta_avg_ydir = cm_delta_avg_ydir_100boca;
	} else {
		cm_delta_xdir = cm_delta_xdir_125boca;
		cm_delta_ydir = cm_delta_ydir_125boca;
		pre_cm_delta_xdir = pre_cm_delta_xdir_125boca;
		pre_cm_delta_ydir = pre_cm_delta_ydir_125boca;
		cm_delta_avg_xdir = cm_delta_avg_xdir_125boca;
		cm_delta_avg_ydir = cm_delta_avg_ydir_125boca;
	}
	input_raw_info(true, &ts->client->dev, "%s: boca %s\n",
			__func__, boca100 ? "100" : "125");

	return boca100;
}

static void sec_ts_cm_spec_over_check(struct sec_ts_data *ts)
{
	int i = 0;
	int j = 0;
	int by, bx, bpos, tpos1, tpos2;
	int vb, vt1, vt2, gapy, gapx, specover_count_x, specover_count_y;
	int xdir_cm_avg[30], ydir_cm_avg[50];
	u8 spec_out_nv = 0x00;
	int data[30];
	int fail_list[6];

	input_info(true, &ts->client->dev, "%s\n", __func__);

	if (sec_ts_get_boca_spec(ts) < 0)
		return;

	input_info(true, &ts->client->dev,
		"rx_max(%d) tx_max(%d)\n", ts->rx_count, ts->tx_count);

	for (i = 0; i < ts->rx_count; i++)
		ydir_cm_avg[i] = 0;
	for (i = 0; i < ts->tx_count; i++)
		xdir_cm_avg[i] = 0;

	specover_count_x = 0;
	specover_count_y = 0;
	for (i = 0; i < 6; i++)
		fail_list[i] = 0;

	input_info(true, &ts->client->dev, "gapX TX\n");

	for (i = 0; i < ts->rx_count; i++) {
		for (j = 0; j < ts->tx_count - 1; j++) {
			bx = i;
			by = j;

			bpos = (j * ts->rx_count) + i;
			tpos2 = ((j + 1) * ts->rx_count) + i;

			vb = ts->pFrame[bpos];
			vt2 = ts->pFrame[tpos2];

			if (vb > vt2)
				gapx = 100 - (vt2 * 100 / vb);
			else
				gapx = 100 - (vb * 100 / vt2);

			/* gapy is  tx delta, a raw of tx delta has to be saved at that row */
			/* bx is  row, by is column, take care */
			ydir_cm_avg[bx] += gapx;
			data[by] = gapx;

			/*
			 * take care  index of x,y.  it is inverted.
			 *input_info(true, &ts->client->dev,
			 *"y(%2d) x(%2d) gapx(%d) cm_spec xdir(%d)\n",bx,by,gapx,cm_delta_xdir[bx][by]);
			 */
			if (ts->tdata->external_factory == false) {
				if (gapx > cm_delta_xdir[bx][by]) {
					specover_count_x++;
					spec_out_nv |= SEC_TS_CM_SPEC_OUT_TX_NODE;
					fail_list[1] = by;
					fail_list[2] = bx;
					input_raw_info(true, &ts->client->dev,
							"y(%2d) x(%2d) gapx(%d) cm_spec xdir(%d)\n",
							bx, by, gapx, cm_delta_xdir[bx][by]);
				}
			} else {
				if (gapx > pre_cm_delta_xdir[bx][by]) {
					specover_count_x++;
					spec_out_nv |= SEC_TS_CM_SPEC_OUT_TX_NODE;
					fail_list[1] = by;
					fail_list[2] = bx;
					input_raw_info(true, &ts->client->dev,
							"y(%2d) x(%2d) gapx(%d) pre_cm_spec xdir(%d)\n",
							bx, by, gapx, pre_cm_delta_xdir[bx][by]);
				}
			}
		}
	}

	input_info(true, &ts->client->dev, "gapY RX\n");

	for (i = 0; i < ts->rx_count - 1; i++) {
		for (j = 0; j < ts->tx_count; j++) {
			bx = i;
			by = j;

			bpos = (j * ts->rx_count) + i;
			tpos1 = (j * ts->rx_count) + (i + 1);

			vb = ts->pFrame[bpos];
			vt1 = ts->pFrame[tpos1];

			if (vb > vt1)
				gapy = 100 - (vt1 * 100 / vb);
			else
				gapy = 100 - (vb * 100 / vt1);

			xdir_cm_avg[by] += gapy;
			data[by] = gapy;

			/*
			 * take care  index of x,y.  it is inverted.
			 *input_info(true, &ts->client->dev,
			 *"y(%2d) x(%2d) gapy(%d) cm_spec ydir(%d)\n",bx,by,gapy,cm_delta_ydir[bx][by]);
			 */
			if (ts->tdata->external_factory == false) {
				if (gapy > cm_delta_ydir[bx][by]) {
					specover_count_y++;
					spec_out_nv |= SEC_TS_CM_SPEC_OUT_RX_NODE;
					fail_list[4] = by;
					fail_list[5] = bx;
					input_raw_info(true, &ts->client->dev,
							"y(%2d) x(%2d) gapy(%d) cm_spec ydir(%d)\n",
							bx, by, gapy, cm_delta_ydir[bx][by]);
				}
			} else {
				if (gapy > pre_cm_delta_ydir[bx][by]) {
					specover_count_y++;
					spec_out_nv |= SEC_TS_CM_SPEC_OUT_RX_NODE;
					fail_list[4] = by;
					fail_list[5] = bx;
					input_raw_info(true, &ts->client->dev,
							"y(%2d) x(%2d) gapy(%d) pre_cm_spec ydir(%d)\n",
							bx, by, gapy, pre_cm_delta_ydir[bx][by]);
				}
			}
		}
	}

	for (i = 0; i < ts->rx_count; i++) {
		/*input_info(true, &ts->client->dev,
		 *""sum(%d) ydir_cm_avg[%2d] = %3d\n", i, ydir_cm_avg[i], ydir_cm_avg[i] / ts->tx_count);
		 */
		if ((ydir_cm_avg[i] / (ts->tx_count - 1)) > cm_delta_avg_ydir[i]) {
			/*specover_count++;*/
			spec_out_nv |= SEC_TS_CM_SPEC_OUT_RX_AVG;
		}
	}
	for (i = 0; i < ts->tx_count; i++) {
		/*input_info(true, &ts->client->dev,
		 *""sum(%d) xdir_cm_avg[%2d] = %3d\n", i,  xdir_cm_avg[i], xdir_cm_avg[i] / (ts->rx_count -1));
		 */
		if ((xdir_cm_avg[i] / (ts->rx_count - 1)) > cm_delta_avg_xdir[i]) {
			/*specover_count++;*/
			spec_out_nv |= SEC_TS_CM_SPEC_OUT_TX_AVG;
		}
	}
	fail_list[0] = specover_count_x;
	fail_list[3] = specover_count_y;

	for (i = 0; i < 6; i++)
		ts->cm_fail_list[i] = fail_list[i];

	ts->tdata->external_factory = false;
	ts->cm_specover = specover_count_x + specover_count_y;
	input_raw_info(true, &ts->client->dev,
			"spec out x(%d) spec out y(%d) nv(0x%02X)\n",
			specover_count_x, specover_count_y, spec_out_nv);
}

static void sec_ts_print_frame(struct sec_ts_data *ts, short *min, short *max)
{
	int i = 0;
	int j = 0;
	unsigned char *pStr = NULL;
	unsigned char pTmp[16] = { 0 };

	input_raw_info(true, &ts->client->dev, "%s\n", __func__);

	pStr = kzalloc(6 * (ts->tx_count + 1), GFP_KERNEL);
	if (pStr == NULL)
		return;

	memset(pStr, 0x0, 6 * (ts->tx_count + 1));
	snprintf(pTmp, sizeof(pTmp), "      TX");
	strncat(pStr, pTmp, 6 * ts->tx_count);

	for (i = 0; i < ts->tx_count; i++) {
		snprintf(pTmp, sizeof(pTmp), " %02d ", i);
		strncat(pStr, pTmp, 6 * ts->tx_count);
	}

	input_raw_info(true, &ts->client->dev, "%s\n", pStr);
	memset(pStr, 0x0, 6 * (ts->tx_count + 1));
	snprintf(pTmp, sizeof(pTmp), " +");
	strncat(pStr, pTmp, 6 * ts->tx_count);

	for (i = 0; i < ts->tx_count; i++) {
		snprintf(pTmp, sizeof(pTmp), "----");
		strncat(pStr, pTmp, 6 * ts->rx_count);
	}

	input_raw_info(true, &ts->client->dev, "%s\n", pStr);

	for (i = 0; i < ts->rx_count; i++) {
		memset(pStr, 0x0, 6 * (ts->tx_count + 1));
		snprintf(pTmp, sizeof(pTmp), "Rx%02d | ", i);
		strncat(pStr, pTmp, 6 * ts->tx_count);

		for (j = 0; j < ts->tx_count; j++) {
			snprintf(pTmp, sizeof(pTmp), " %3d", ts->pFrame[(j * ts->rx_count) + i]);

			if (i > 0) {
				if (ts->pFrame[(j * ts->rx_count) + i] < *min)
					*min = ts->pFrame[(j * ts->rx_count) + i];

				if (ts->pFrame[(j * ts->rx_count) + i] > *max)
					*max = ts->pFrame[(j * ts->rx_count) + i];
			}
			strncat(pStr, pTmp, 6 * ts->rx_count);
		}
		input_raw_info(true, &ts->client->dev, "%s\n", pStr);
	}
	kfree(pStr);
}

static int sec_ts_read_frame(struct sec_ts_data *ts, u8 type, short *min,
				short *max, bool save_result)
{
	unsigned int readbytes = 0xFF;
	unsigned char *pRead = NULL;
	u8 mode = TYPE_INVALID_DATA;
	int ret = 0;
	int i = 0;
	int j = 0;
	short *temp = NULL;

	input_raw_info(true, &ts->client->dev, "%s: type %d\n", __func__, type);

	/* set data length, allocation buffer memory */
	readbytes = ts->rx_count * ts->tx_count * 2;

	pRead = kzalloc(readbytes, GFP_KERNEL);
	if (!pRead)
		return -ENOMEM;

	/* set OPCODE and data type */
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_MUTU_RAW_TYPE, &type, 1);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: Set rawdata type failed\n", __func__);
		goto ErrorExit;
	}

	sec_ts_delay(30);

	if (type == TYPE_OFFSET_DATA_SDC) {
		/* excute selftest for real cap offset data, because real cap data is not memory data in normal touch. */
		char para = TO_TOUCH_MODE;

		disable_irq(ts->client->irq);

		execute_selftest(ts, save_result);

		ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, &para, 1);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: Set rawdata type failed\n", __func__);
			enable_irq(ts->client->irq);
			goto ErrorRelease;
		}

		enable_irq(ts->client->irq);
	}

	/* read data */
	ret = ts->sec_ts_i2c_read(ts, SEC_TS_READ_TOUCH_RAWDATA, pRead, readbytes);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: read rawdata failed!\n", __func__);
		goto ErrorRelease;
	}

	memset(ts->pFrame, 0x00, readbytes);

	for (i = 0; i < readbytes; i += 2)
		ts->pFrame[i / 2] = pRead[i + 1] + (pRead[i] << 8);

	*min = *max = ts->pFrame[0];

#ifdef DEBUG_MSG
	input_info(true, &ts->client->dev, "%s: 02X%02X%02X readbytes=%d\n", __func__,
			pRead[0], pRead[1], pRead[2], readbytes);
#endif
	sec_ts_print_frame(ts, min, max);

	if (type == TYPE_OFFSET_DATA_SDC)
		sec_ts_cm_spec_over_check(ts);
	else if (type == TYPE_RAW_DATA_P2P_MIN) {
		if(ts->pminFrame) {
			for (i = 0; i < ts->rx_count * ts->tx_count; i++)
				ts->pminFrame[i] = ts->pFrame[i];
		}
	} else if (type == TYPE_RAW_DATA_P2P_MAX) {
		if(ts->pmaxFrame) {
			for (i = 0; i < ts->rx_count * ts->tx_count; i++)
				ts->pmaxFrame[i] = ts->pFrame[i];
		}
	}

	temp = kzalloc(readbytes, GFP_KERNEL);
	if (!temp)
		goto ErrorRelease;

	memcpy(temp, ts->pFrame, ts->tx_count * ts->rx_count * 2);
	memset(ts->pFrame, 0x00, ts->tx_count * ts->rx_count * 2);

	for (i = 0; i < ts->tx_count; i++) {
		for (j = 0; j < ts->rx_count; j++)
			ts->pFrame[(j * ts->tx_count) + i] = temp[(i * ts->rx_count) + j];
	}

	kfree(temp);

ErrorRelease:
	/* release data monitory (unprepare AFE data memory) */
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_MUTU_RAW_TYPE, &mode, 1);
	if (ret < 0)
		input_err(true, &ts->client->dev, "%s: Set rawdata type failed\n", __func__);

ErrorExit:
	kfree(pRead);

	return ret;
}

static void sec_ts_print_channel(struct sec_ts_data *ts) {
	unsigned char *pStr = NULL;
	unsigned char pTmp[16] = { 0 };
	int i = 0, j = 0, k = 0;

	if (!ts->tx_count)
		return;

	pStr = vzalloc(7 * (ts->tx_count + 1));
	if (!pStr)
		return;

	memset(pStr, 0x0, 7 * (ts->tx_count + 1));
	snprintf(pTmp, sizeof(pTmp), " TX");
	strncat(pStr, pTmp, 7 * ts->tx_count);

	for (k = 0; k < ts->tx_count; k++) {
		snprintf(pTmp, sizeof(pTmp), "    %02d", k);
		strncat(pStr, pTmp, 7 * ts->tx_count);
	}
	input_raw_info(true, &ts->client->dev, "%s\n", pStr);

	memset(pStr, 0x0, 7 * (ts->tx_count + 1));
	snprintf(pTmp, sizeof(pTmp), " +");
	strncat(pStr, pTmp, 7 * ts->tx_count);

	for (k = 0; k < ts->tx_count; k++) {
		snprintf(pTmp, sizeof(pTmp), "------");
		strncat(pStr, pTmp, 7 * ts->tx_count);
	}
	input_raw_info(true, &ts->client->dev, "%s\n", pStr);

	memset(pStr, 0x0, 7 * (ts->tx_count + 1));
	snprintf(pTmp, sizeof(pTmp), " | ");
	strncat(pStr, pTmp, 7 * ts->tx_count);

	for (i = 0; i < (ts->tx_count + ts->rx_count) * 2; i += 2) {
		if (j == ts->tx_count) {
			input_raw_info(true, &ts->client->dev, "%s\n", pStr);
			input_raw_info(true, &ts->client->dev, "\n");
			memset(pStr, 0x0, 7 * (ts->tx_count + 1));
			snprintf(pTmp, sizeof(pTmp), " RX");
			strncat(pStr, pTmp, 7 * ts->tx_count);

			for (k = 0; k < ts->tx_count; k++) {
				snprintf(pTmp, sizeof(pTmp), "    %02d", k);
				strncat(pStr, pTmp, 7 * ts->tx_count);
			}

			input_raw_info(true, &ts->client->dev, "%s\n", pStr);

			memset(pStr, 0x0, 7 * (ts->tx_count + 1));
			snprintf(pTmp, sizeof(pTmp), " +");
			strncat(pStr, pTmp, 7 * ts->tx_count);

			for (k = 0; k < ts->tx_count; k++) {
				snprintf(pTmp, sizeof(pTmp), "------");
				strncat(pStr, pTmp, 7 * ts->tx_count);
			}
			input_raw_info(true, &ts->client->dev, "%s\n", pStr);

			memset(pStr, 0x0, 7 * (ts->tx_count + 1));
			snprintf(pTmp, sizeof(pTmp), " | ");
			strncat(pStr, pTmp, 7 * ts->tx_count);
		} else if (j && !(j % ts->tx_count)) {
			input_raw_info(true, &ts->client->dev, "%s\n", pStr);
			memset(pStr, 0x0, 7 * (ts->tx_count + 1));
			snprintf(pTmp, sizeof(pTmp), " | ");
			strncat(pStr, pTmp, 7 * ts->tx_count);
		}

		snprintf(pTmp, sizeof(pTmp), " %5d", ts->pFrame[j]);
		strncat(pStr, pTmp, 7 * ts->tx_count);

		j++;
	}
	input_raw_info(true, &ts->client->dev, "%s\n", pStr);
	vfree(pStr);
}

static int sec_ts_read_channel(struct sec_ts_data *ts, u8 type,
				short *min, short *max, bool save_result)
{
	unsigned char *pRead = NULL;
	u8 mode = TYPE_INVALID_DATA;
	int ret = 0;
	int ii = 0;
	int jj = 0;
	unsigned int data_length = (ts->tx_count + ts->rx_count) * 2;
	u8 w_data;

	input_raw_info(true, &ts->client->dev, "%s: type %d\n", __func__, type);

	pRead = kzalloc(data_length, GFP_KERNEL);
	if (!pRead)
		return -ENOMEM;

	/* set OPCODE and data type */
	w_data = type;

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SELF_RAW_TYPE, &w_data, 1);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: Set rawdata type failed\n", __func__);
		goto out_read_channel;
	}

	sec_ts_delay(50);

	if (type == TYPE_OFFSET_DATA_SDC) {
		/* excute selftest for real cap offset data, because real cap data is not memory data in normal touch. */
		char para = TO_TOUCH_MODE;
		disable_irq(ts->client->irq);
		execute_selftest(ts, save_result);
		ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, &para, 1);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: set rawdata type failed!\n", __func__);
			enable_irq(ts->client->irq);
			goto err_read_data;
		}
		enable_irq(ts->client->irq);
		/* end */
	}

	/* read data */
	ret = ts->sec_ts_i2c_read(ts, SEC_TS_READ_TOUCH_SELF_RAWDATA, pRead, data_length);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: read rawdata failed!\n", __func__);
		goto err_read_data;
	}

	/* clear all pFrame data */
	memset(ts->pFrame, 0x00, data_length);

	for (ii = 0; ii < data_length; ii += 2) {
		ts->pFrame[jj] = ((pRead[ii] << 8) | pRead[ii + 1]);

		if (ii == 0)
			*min = *max = ts->pFrame[jj];

		*min = min(*min, ts->pFrame[jj]);
		*max = max(*max, ts->pFrame[jj]);

		jj++;
	}

	if(type == TYPE_RAW_DATA_P2P_MIN || type == TYPE_RAW_DATA_P2P_MAX) {
		// cal TX CSR
		ts->self_raw_set_p2p_y_min = 0xffff;
		ts->self_raw_set_p2p_y_max = 0;
		for(ii = 0; ii < ts->tx_count; ii++) {
			if(ts->pminFrame[ii] < ts->self_raw_set_p2p_y_min)
				ts->self_raw_set_p2p_y_min = ts->pminFrame[ii];
			if(ts->pminFrame[ii] > ts->self_raw_set_p2p_y_max)
				ts->self_raw_set_p2p_y_max = ts->pminFrame[ii];
		}
		// cal RX CSR
		ts->self_raw_set_p2p_x_min = 0xffff;
		ts->self_raw_set_p2p_x_max = 0;
		for(ii = ts->tx_count; ii < ts->tx_count + ts->rx_count; ii++) {
			if(ts->pminFrame[ii] < ts->self_raw_set_p2p_x_min)
				ts->self_raw_set_p2p_x_min = ts->pminFrame[ii];
			if(ts->pminFrame[ii] < ts->self_raw_set_p2p_x_max)
				ts->self_raw_set_p2p_x_max = ts->pminFrame[ii];
		}
	}

	if(type == TYPE_RAW_DATA_P2P_MIN) {
		for(ii = 0; ii < ts->tx_count + ts->rx_count; ii++)
			ts->pminFrame[ii] = ts->pFrame[ii];
	} else if(type == TYPE_RAW_DATA_P2P_MAX) {
		for(ii = 0; ii < ts->tx_count + ts->rx_count; ii++)
			ts->pmaxFrame[ii] = ts->pFrame[ii];
	}

	sec_ts_print_channel(ts);

err_read_data:
	/* release data monitory (unprepare AFE data memory) */
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SELF_RAW_TYPE, &mode, 1);
	if (ret < 0)
		input_err(true, &ts->client->dev, "%s: Set rawdata type failed\n", __func__);

out_read_channel:
	kfree(pRead);

	return ret;
}

static int get_gap_data(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };
	int ii;
	int node_gap_tx = 0;
	int node_gap_rx = 0;
	int tx_max = 0;
	int rx_max = 0;

	for (ii = 0; ii < (ts->rx_count * ts->tx_count); ii++) {
		if ((ii + 1) % (ts->tx_count) != 0) {
			if (ts->pFrame[ii] > ts->pFrame[ii + 1])
				node_gap_tx = 100 - (ts->pFrame[ii + 1] * 100 / ts->pFrame[ii]);
			else
				node_gap_tx = 100 - (ts->pFrame[ii] * 100 / ts->pFrame[ii + 1]);
			tx_max = max(tx_max, node_gap_tx);
		}

		if (ii < (ts->rx_count - 1) * ts->tx_count) {
			if (ts->pFrame[ii] > ts->pFrame[ii + ts->tx_count])
				node_gap_rx = 100 - (ts->pFrame[ii + ts->tx_count] * 100 / ts->pFrame[ii]);
			else
				node_gap_rx = 100 - (ts->pFrame[ii] * 100 / ts->pFrame[ii + ts->tx_count]);
			rx_max = max(rx_max, node_gap_rx);
		}
	}

	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING) {
		snprintf(buff, sizeof(buff), "%d,%d", 0, tx_max);
		sec_cmd_set_cmd_result_all(sec, buff, SEC_CMD_STR_LEN, "GAP_DATA_X");
		snprintf(buff, sizeof(buff), "%d,%d", 0, rx_max);
		sec_cmd_set_cmd_result_all(sec, buff, SEC_CMD_STR_LEN, "GAP_DATA_Y");
	}

	sec->cmd_state = SEC_CMD_STATUS_OK;
	return 0;
}

static void get_gap_data_x_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char *buff = NULL;
	int ii;
	int node_gap = 0;
	char temp[SEC_CMD_STR_LEN] = { 0 };

	sec_cmd_set_default_result(sec);

	buff = kzalloc(ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN, GFP_KERNEL);
	if (!buff)
		return;

	for (ii = 0; ii < (ts->rx_count * ts->tx_count); ii++) {
		if ((ii + 1) % (ts->tx_count) != 0) {
			if (ts->pFrame[ii] > ts->pFrame[ii + 1])
				node_gap = 100 - (ts->pFrame[ii + 1] * 100 / ts->pFrame[ii]);
			else
				node_gap = 100 - (ts->pFrame[ii] * 100 / ts->pFrame[ii + 1]);

			snprintf(temp, CMD_RESULT_WORD_LEN, "%d,", node_gap);
			strncat(buff, temp, CMD_RESULT_WORD_LEN);
			memset(temp, 0x00, SEC_CMD_STR_LEN);
		}
	}

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	kfree(buff);
}

static void get_gap_data_y_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char *buff = NULL;
	int ii;
	int node_gap = 0;
	char temp[SEC_CMD_STR_LEN] = { 0 };

	sec_cmd_set_default_result(sec);

	buff = kzalloc(ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN, GFP_KERNEL);
	if (!buff)
		return;

	for (ii = 0; ii < (ts->rx_count * ts->tx_count); ii++) {
		if (ii < (ts->rx_count - 1) * ts->tx_count) {
			if (ts->pFrame[ii] > ts->pFrame[ii + ts->tx_count])
				node_gap = 100 - (ts->pFrame[ii + ts->tx_count] * 100 / ts->pFrame[ii]);
			else
				node_gap = 100 - (ts->pFrame[ii] * 100 / ts->pFrame[ii + ts->tx_count]);

			snprintf(temp, CMD_RESULT_WORD_LEN, "%d,", node_gap);
			strncat(buff, temp, CMD_RESULT_WORD_LEN);
			memset(temp, 0x00, SEC_CMD_STR_LEN);
		}
	}

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	kfree(buff);
}

static int get_self_channel_data(void *device_data, u8 type)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };
	int ii;
	short tx_min = 0;
	short rx_min = 0;
	short tx_max = 0;
	short rx_max = 0;
	char *item_name = "NULL";
	char temp[SEC_CMD_STR_LEN] = { 0 };

	switch (type) {
	case TYPE_OFFSET_DATA_SDC:
		item_name = "SELF_OFFSET_MODULE";
		break;
	case TYPE_RAW_DATA:
		item_name = "SELF_DELTA";
		break;
	case TYPE_OFFSET_DATA_SEC:
		item_name = "SELF_OFFSET_SET";
		break;
	default:
		break;
	}

	for (ii = 0; ii < ts->tx_count; ii++) {
		if (ii == 0)
			tx_min = tx_max = ts->pFrame[ii];

		tx_min = min(tx_min, ts->pFrame[ii]);
		tx_max = max(tx_max, ts->pFrame[ii]);
	}
	snprintf(buff, sizeof(buff), "%d,%d", tx_min, tx_max);
	snprintf(temp, sizeof(temp), "%s%s", item_name, "_Y");
	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING)
		sec_cmd_set_cmd_result_all(sec, buff, sizeof(buff), temp);

	memset(temp, 0x00, SEC_CMD_STR_LEN);

	for (ii = ts->tx_count; ii < ts->tx_count + ts->rx_count; ii++) {
		if (ii == ts->tx_count)
			rx_min = rx_max = ts->pFrame[ii];

		rx_min = min(rx_min, ts->pFrame[ii]);
		rx_max = max(rx_max, ts->pFrame[ii]);
	}
	snprintf(buff, sizeof(buff), "%d,%d", rx_min, rx_max);
	snprintf(temp, sizeof(temp), "%s%s", item_name, "_X");
	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING)
		sec_cmd_set_cmd_result_all(sec, buff, sizeof(buff), temp);

	return 0;
}

static int sec_ts_read_raw_data(struct sec_ts_data *ts,
		struct sec_cmd_data *sec, struct sec_ts_test_mode *mode)
{
	int ii;
	int ret = 0;
	char temp[SEC_CMD_STR_LEN] = { 0 };
	char *buff;
	char *item_name = "NULL";

	switch (mode->type) {
	case TYPE_OFFSET_DATA_SDC:
		item_name = "CM_OFFSET_MODULE";
		break;
	case TYPE_RAW_DATA:
		item_name = "CM_DELTA";
		break;
	case TYPE_OFFSET_DATA_SEC:
		item_name = "CM_OFFSET_SET";
		break;
	default:
		break;
	}

	buff = kzalloc(ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN, GFP_KERNEL);
	if (!buff)
		goto error_alloc_mem;

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		goto error_power_state;
	}

	input_raw_info(true, &ts->client->dev, "%s: %d, %s\n",
			__func__, mode->type, mode->allnode ? "ALL" : "");

	ret = sec_ts_fix_tmode(ts, TOUCH_SYSTEM_MODE_TOUCH, TOUCH_MODE_STATE_TOUCH);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: failed to fix tmode\n",
				__func__);
		goto error_test_fail;
	}

	if (mode->frame_channel)
		ret = sec_ts_read_channel(ts, mode->type, &mode->min, &mode->max, true);
	else
		ret = sec_ts_read_frame(ts, mode->type, &mode->min, &mode->max, true);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: failed to read frame\n",
				__func__);
		goto error_test_fail;
	}

	if (mode->allnode) {
		if (mode->frame_channel) {
			for (ii = 0; ii < (ts->rx_count + ts->tx_count); ii++) {
				snprintf(temp, CMD_RESULT_WORD_LEN, "%d,", ts->pFrame[ii]);
				strncat(buff, temp, CMD_RESULT_WORD_LEN);

				memset(temp, 0x00, SEC_CMD_STR_LEN);
			}
		} else {
			for (ii = 0; ii < (ts->rx_count * ts->tx_count); ii++) {
				snprintf(temp, CMD_RESULT_WORD_LEN, "%d,", ts->pFrame[ii]);
				strncat(buff, temp, CMD_RESULT_WORD_LEN);

				memset(temp, 0x00, SEC_CMD_STR_LEN);
			}
		}
	} else {
		snprintf(buff, SEC_CMD_STR_LEN, "%d,%d", mode->min, mode->max);
	}

	ret = sec_ts_release_tmode(ts);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: failed to release tmode\n",
				__func__);
		goto error_test_fail;
	}

	if (!sec)
		goto out_rawdata;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN));

	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING && (!mode->frame_channel))
		sec_cmd_set_cmd_result_all(sec, buff,
				strnlen(buff, ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN), item_name);

	sec->cmd_state = SEC_CMD_STATUS_OK;

out_rawdata:
	kfree(buff);

	sec_ts_locked_release_all_finger(ts);

	return ret;

error_test_fail:
error_power_state:
	kfree(buff);
error_alloc_mem:
	if (!sec)
		return ret;

	snprintf(temp, SEC_CMD_STR_LEN, "FAIL");
	sec_cmd_set_cmd_result(sec, temp, SEC_CMD_STR_LEN);
	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING && (!mode->frame_channel))
		sec_cmd_set_cmd_result_all(sec, temp, SEC_CMD_STR_LEN, item_name);
	sec->cmd_state = SEC_CMD_STATUS_FAIL;

	sec_ts_locked_release_all_finger(ts);

	return ret;
}

static int sec_ts_read_rawp2p_data(struct sec_ts_data *ts,
		struct sec_cmd_data *sec, struct sec_ts_test_mode *mode)
{
	int ii;
	int ret = 0;
	char temp[SEC_CMD_STR_LEN] = { 0 };
	char *buff;
	char para = TO_TOUCH_MODE;

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		goto error_power_state;
	}

	buff = kzalloc(ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN, GFP_KERNEL);
	if (!buff)
		goto error_alloc_mem;

	input_info(true, &ts->client->dev, "%s: %d, %s\n",
			__func__, mode->type, mode->allnode ? "ALL" : "");

	disable_irq(ts->client->irq);

	ret = sec_ts_p2p_tmode(ts);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: failed to fix p2p tmode\n",
				__func__);
		goto error_tmode_fail;
	}

	ret = execute_p2ptest(ts);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: failed to fix p2p test\n",
				__func__);
		goto error_test_fail;
	}

	if (mode->frame_channel)
		ret = sec_ts_read_channel(ts, mode->type, &mode->min, &mode->max, true);
	else
		ret = sec_ts_read_frame(ts, mode->type, &mode->min, &mode->max, true);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: failed to read frame\n",
				__func__);
		goto error_test_fail;
	}

	if (mode->allnode) {
		if (mode->frame_channel) {
			for (ii = 0; ii < (ts->rx_count + ts->tx_count); ii++) {
				snprintf(temp, CMD_RESULT_WORD_LEN, "%d,", ts->pFrame[ii]);
				strncat(buff, temp, CMD_RESULT_WORD_LEN);

				memset(temp, 0x00, SEC_CMD_STR_LEN);
			}
		} else {
			for (ii = 0; ii < (ts->rx_count * ts->tx_count); ii++) {
				snprintf(temp, CMD_RESULT_WORD_LEN, "%d,", ts->pFrame[ii]);
				strncat(buff, temp, CMD_RESULT_WORD_LEN);

				memset(temp, 0x00, SEC_CMD_STR_LEN);
			}
		}
	} else {
		snprintf(buff, SEC_CMD_STR_LEN, "%d,%d", mode->min, mode->max);
	}

	if (!sec)
		goto out_rawdata;

	sec_ts_locked_release_all_finger(ts);
	sec_ts_delay(30);
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, &para, 1);
	if (ret < 0)
		input_err(true, &ts->client->dev, "%s: set rawdata type failed!\n", __func__);

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN));
	sec->cmd_state = SEC_CMD_STATUS_OK;

out_rawdata:
	enable_irq(ts->client->irq);
	kfree(buff);

	return ret;
error_test_fail:
error_tmode_fail:
	kfree(buff);
	enable_irq(ts->client->irq);
error_alloc_mem:
error_power_state:
	if (!sec)
		return ret;

	snprintf(temp, SEC_CMD_STR_LEN, "FAIL");
	sec_cmd_set_cmd_result(sec, temp, SEC_CMD_STR_LEN);
	sec->cmd_state = SEC_CMD_STATUS_FAIL;

	return ret;
}

static int sec_ts_read_rawp2p_data_all(struct sec_ts_data *ts,
		struct sec_cmd_data *sec, struct sec_ts_test_mode *mode)
{
	int ret = 0;
	char buff[SEC_CMD_STR_LEN] = { 0 };
	char para = TO_TOUCH_MODE;

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		goto error_power_state;
	}

	input_info(true, &ts->client->dev, "%s: start\n", __func__);

	disable_irq(ts->client->irq);
	ret = sec_ts_p2p_tmode(ts);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: failed to fix p2p tmode\n",
				__func__);
		goto error_tmode_fail;
	}

	ret = execute_p2ptest(ts);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: failed to fix p2p test\n",
				__func__);
		goto error_test_fail;
	}

	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING) {
		snprintf(buff, sizeof(buff), "%d,%d", ts->cm_raw_set_p2p_min, ts->cm_raw_set_p2p_max);
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "CM_RAW_SET_P2P");
		snprintf(buff, sizeof(buff), "%d,%d", 0, ts->cm_raw_set_p2p_diff);
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "CM_RAW_SET_P2P_DIFF");
		snprintf(buff, sizeof(buff), "%d,%d", ts->self_raw_set_p2p_x_min, ts->self_raw_set_p2p_x_max);
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "SELF_RAW_SET_P2P_X");
		snprintf(buff, sizeof(buff), "%d,%d", ts->self_raw_set_p2p_y_min, ts->self_raw_set_p2p_y_max);
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "SELF_RAW_SET_P2P_Y");
		snprintf(buff, sizeof(buff), "%d,%d", 0, ts->self_raw_set_p2p_x_diff);
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "SELF_RAW_SET_P2P_X_DIFF");
		snprintf(buff, sizeof(buff), "%d,%d", 0, ts->self_raw_set_p2p_y_diff);
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "SELF_RAW_SET_P2P_Y_DIFF");
	}
	sec->cmd_state = SEC_CMD_STATUS_OK;

	sec_ts_locked_release_all_finger(ts);

	sec_ts_delay(30);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, &para, 1);
	if (ret < 0)
		input_err(true, &ts->client->dev, "%s: set rawdata type failed!\n", __func__);

	enable_irq(ts->client->irq);

	return ret;

error_test_fail:
	sec_ts_locked_release_all_finger(ts);
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, &para, 1);
	if (ret < 0)
		input_err(true, &ts->client->dev, "%s: set rawdata type failed!\n", __func__);

error_tmode_fail:
	enable_irq(ts->client->irq);
error_power_state:
	snprintf(buff, sizeof(buff), "%s", "FAIL");
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));

	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING) {
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "CM_RAW_SET_P2P");
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "CM_RAW_SET_P2P_DIFF");
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "SELF_RAW_SET_P2P_X");
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "SELF_RAW_SET_P2P_Y");
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "SELF_RAW_SET_P2P_X_DIFF");
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "SELF_RAW_SET_P2P_Y_DIFF");
	}
	sec->cmd_state = SEC_CMD_STATUS_FAIL;
	return ret;
}

static void get_fw_ver_bin(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };

	sec_cmd_set_default_result(sec);

	snprintf(buff, sizeof(buff), "SE%02X%02X%02X",
			ts->plat_data->panel_revision, ts->plat_data->img_version_of_bin[2],
			ts->plat_data->img_version_of_bin[3]);

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING)
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "FW_VER_BIN");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void get_fw_ver_ic(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };
	int ret;
	u8 fw_ver[4];

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		snprintf(buff, sizeof(buff), "%s", "TSP turned off");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		return;
	}

	ret = ts->sec_ts_i2c_read(ts, SEC_TS_READ_IMG_VERSION, fw_ver, 4);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: firmware version read error\n", __func__);
		snprintf(buff, sizeof(buff), "%s", "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		return;
	}

	snprintf(buff, sizeof(buff), "SE%02X%02X%02X",
			ts->plat_data->panel_revision, fw_ver[2], fw_ver[3]);

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING)
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "FW_VER_IC");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void get_cm_spec_over(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };

	sec_cmd_set_default_result(sec);

	snprintf(buff, sizeof(buff), "%d %d %d %d %d %d",
		ts->cm_fail_list[0], ts->cm_fail_list[1], ts->cm_fail_list[2],
		ts->cm_fail_list[3], ts->cm_fail_list[4], ts->cm_fail_list[5]);

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void get_boca_type(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };
	int boca100 = 0;

	sec_cmd_set_default_result(sec);

	boca100 = sec_ts_get_boca_spec(ts);
	if (boca100 < 0) {
		snprintf(buff, sizeof(buff), "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		return;
	}

	snprintf(buff, sizeof(buff), "%d", boca100);
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void get_config_ver(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[22] = { 0 };

	sec_cmd_set_default_result(sec);

	snprintf(buff, sizeof(buff), "%s_SE_%02X%02X",
			ts->plat_data->model_name,
			ts->plat_data->config_version_of_ic[2], ts->plat_data->config_version_of_ic[3]);

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

#ifdef TCLM_CONCEPT
static void get_pat_information(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[50] = { 0 };

	sec_cmd_set_default_result(sec);

	/* fixed tune version will be saved at excute autotune */
	snprintf(buff, sizeof(buff), "C%02XT01%02X.%4s%s%s",
		ts->tdata->cal_count, ts->tdata->tune_fix_ver, ts->tdata->tclm_string[ts->tdata->cal_position].f_name,
		(ts->tdata->tclm_level == TCLM_LEVEL_LOCKDOWN) ? ".L " : " ", ts->tdata->cal_pos_hist_last3);

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void set_external_factory(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[22] = { 0 };

	sec_cmd_set_default_result(sec);

	ts->tdata->external_factory = true;
	snprintf(buff, sizeof(buff), "OK");

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}
#endif

static void get_threshold(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[20] = { 0 };
	char threshold[2] = { 0 };
	int ret;

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n", __func__);
		snprintf(buff, sizeof(buff), "%s", "TSP turned off");
		goto err;
	}

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_TOUCH_MODE_FOR_THRESHOLD, threshold, 1);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: threshold write type failed. ret: %d\n", __func__, ret);
		snprintf(buff, sizeof(buff), "%s", "NG");
		goto err;
	}

	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_TOUCH_THRESHOLD, threshold, 2);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: read threshold fail!\n", __func__);
		snprintf(buff, sizeof(buff), "%s", "NG");
		goto err;
	}

	input_info(true, &ts->client->dev, "0x%02X, 0x%02X\n",
			threshold[0], threshold[1]);

	snprintf(buff, sizeof(buff), "%d", (threshold[0] << 8) | threshold[1]);

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);

	return;
err:
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
	return;
}

static void module_off_master(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[3] = { 0 };
	int ret = 0;

	ret = sec_ts_stop_device(ts);

	if (ret == 0)
		snprintf(buff, sizeof(buff), "%s", "OK");
	else
		snprintf(buff, sizeof(buff), "%s", "NG");

	sec_cmd_set_default_result(sec);
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	if (strncmp(buff, "OK", 2) == 0)
		sec->cmd_state = SEC_CMD_STATUS_OK;
	else
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void module_on_master(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[3] = { 0 };
	int ret = 0;

	ret = sec_ts_start_device(ts);

	if (ts->input_dev->disabled) {
		sec_ts_set_lowpowermode(ts, TO_LOWPOWER_MODE);
		ts->power_status = SEC_TS_STATE_LPM;
	}

	if (ret == 0)
		snprintf(buff, sizeof(buff), "%s", "OK");
	else
		snprintf(buff, sizeof(buff), "%s", "NG");

	sec_cmd_set_default_result(sec);
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	if (strncmp(buff, "OK", 2) == 0)
		sec->cmd_state = SEC_CMD_STATUS_OK;
	else
		sec->cmd_state = SEC_CMD_STATUS_FAIL;

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void get_chip_vendor(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };

	strncpy(buff, "SEC", sizeof(buff));
	sec_cmd_set_default_result(sec);
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING)
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "IC_VENDOR");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void get_chip_name(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };

	if (ts->plat_data->img_version_of_ic[0] == 0x02)
		strncpy(buff, "MC44", sizeof(buff));
	else if (ts->plat_data->img_version_of_ic[0] == 0x05)
		strncpy(buff, "A552", sizeof(buff));
	else if (ts->plat_data->img_version_of_ic[0] == 0x09)
		strncpy(buff, "Y661", sizeof(buff));
	else if (ts->plat_data->img_version_of_ic[0] == 0x10)
		strncpy(buff, "Y761", sizeof(buff));
	else
		strncpy(buff, "N/A", sizeof(buff));

	sec_cmd_set_default_result(sec);
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING)
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "IC_NAME");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void set_mis_cal_spec(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };
	char wreg[5] = { 0 };
	int ret;

	sec_cmd_set_default_result(sec);

	if (ts->plat_data->mis_cal_check == 0) {
		input_err(true, &ts->client->dev, "%s: [ERROR] not support, %d\n", __func__);
		goto NG;
	} else if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n", __func__);
		goto NG;
	} else {
		if ((sec->cmd_param[0] < 0 || sec->cmd_param[0] > 255) ||
				(sec->cmd_param[1] < 0 || sec->cmd_param[1] > 255) ||
				(sec->cmd_param[2] < 0 || sec->cmd_param[2] > 255)) {
			snprintf(buff, sizeof(buff), "%s", "NG");
			goto NG;
		} else {
			wreg[0] = sec->cmd_param[0];
			wreg[1] = sec->cmd_param[1];
			wreg[2] = sec->cmd_param[2];

			ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_MIS_CAL_SPEC, wreg, 3);
			if (ret < 0) {
				input_err(true, &ts->client->dev, "%s: nvm write failed. ret: %d\n", __func__, ret);
				goto NG;
			} else {
				input_info(true, &ts->client->dev, "%s: tx gap=%d, rx gap=%d, peak=%d\n", __func__, wreg[0], wreg[1], wreg[2]);
				sec_ts_delay(20);
			}
		}
	}

	snprintf(buff, sizeof(buff), "%s", "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
	return;

NG:
	snprintf(buff, sizeof(buff), "%s", "NG");
	sec->cmd_state = SEC_CMD_STATUS_FAIL;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
	return;


}

/*
 *	## Mis Cal result ##
 *	FF : initial value in Firmware.
 *	FD : Cal fail case
 *	F4 : i2c fail case (5F)
 *	F3 : i2c fail case (5E)
 *	F2 : power off state
 *	F1 : not support mis cal concept
 *	F0 : initial value in fucntion
 *	08 : Ambient Ambient condition check(PEAK) result 0 (PASS), 1(FAIL)
 *	04 : Ambient Ambient condition check(DIFF MAX TX) result 0 (PASS), 1(FAIL)
 *	02 : Ambient Ambient condition check(DIFF MAX RX) result 0 (PASS), 1(FAIL)
 *	01 : Wet Wet mode result 0 (PASS), 1(FAIL)
 *	00 : Pass
 */
static void get_mis_cal_info(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };
	char mis_cal_data = 0xF0;
	char wreg[5] = { 0 };
	int ret;
	char buff_all[16] = { 0 };

	sec_cmd_set_default_result(sec);

	if (ts->plat_data->mis_cal_check == 0) {
		input_err(true, &ts->client->dev, "%s: [ERROR] not support, %d\n", __func__);
		mis_cal_data = 0xF1;
		goto NG;
	} else if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n", __func__);
		mis_cal_data = 0xF2;
		goto NG;
	} else {
		ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_MIS_CAL_READ, &mis_cal_data, 1);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: i2c fail!, %d\n", __func__, ret);
			mis_cal_data = 0xF3;
			goto NG;
		} else {
			input_info(true, &ts->client->dev, "%s: miss cal data : %d\n", __func__, mis_cal_data);
		}

		ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_MIS_CAL_SPEC, wreg, 3);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: i2c fail!, %d\n", __func__, ret);
			mis_cal_data = 0xF4;
			goto NG;
		} else {
			input_info(true, &ts->client->dev, "%s: miss cal spec : %d,%d,%d\n", __func__,
					wreg[0], wreg[1], wreg[2]);
		}
	}

	snprintf(buff, sizeof(buff), "%d", mis_cal_data);
	snprintf(buff_all, sizeof(buff_all), "%d,%d,%d,%d", mis_cal_data, wreg[0], wreg[1], wreg[2]);

	sec_cmd_set_cmd_result(sec, buff_all, strnlen(buff_all, sizeof(buff_all)));
	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING)
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "MIS_CAL");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff_all);
	return;

NG:
	snprintf(buff, sizeof(buff), "%d", mis_cal_data);
	snprintf(buff_all, sizeof(buff_all), "%d,%d,%d,%d", mis_cal_data, 0, 0, 0);
	sec_cmd_set_cmd_result(sec, buff_all, strnlen(buff_all, sizeof(buff_all)));
	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING)
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "MIS_CAL");
	sec->cmd_state = SEC_CMD_STATUS_FAIL;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff_all);
}

static void get_mis_cal_info_main(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };
	char mis_cal_data = 0xF0;
	char wreg[5] = { 0 };
	int ret;
	char buff_all[16] = { 0 };

	sec_cmd_set_default_result(sec);

	if (ts->plat_data->mis_cal_check == 0) {
		input_err(true, &ts->client->dev, "%s: [ERROR] not support, %d\n", __func__);
		mis_cal_data = 0xF1;
		goto NG;
	} else if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n", __func__);
		mis_cal_data = 0xF2;
		goto NG;
	} else {
		ret = sec_ts_fix_tmode(ts, TOUCH_SYSTEM_MODE_TOUCH, TOUCH_MODE_STATE_TOUCH);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: failed to fix tmode\n",
				__func__);
			mis_cal_data = 0xF3;
			goto NG;
		}
		wreg[0] = 0x5A;
		wreg[1] = 0x5A;
		wreg[2] = 0x50;
		
		ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_MIS_CAL_SPEC, wreg, 3);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: nvm write failed. ret: %d\n", __func__, ret);
			mis_cal_data = 0xF3;
			goto NG;
		} else {
			input_info(true, &ts->client->dev, "%s: tx gap=%d, rx gap=%d, peak=%d\n", __func__, wreg[0], wreg[1], wreg[2]);
			sec_ts_delay(20);
		}
		ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_MIS_CAL_CHECK, NULL, 0);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: nvm write failed. ret: %d\n", __func__, ret);
			mis_cal_data = 0xF3;
			goto NG;
		}
		sec_ts_delay(200);
	
		ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_MIS_CAL_READ, &mis_cal_data, 1);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: i2c fail!, %d\n", __func__, ret);
			mis_cal_data = 0xF3;
			goto NG;
		} else {
			input_info(true, &ts->client->dev, "%s: miss cal data : %d\n", __func__, mis_cal_data);
		}

		ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_MIS_CAL_SPEC, wreg, 3);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: i2c fail!, %d\n", __func__, ret);
			mis_cal_data = 0xF4;
			goto NG;
		} else {
			input_info(true, &ts->client->dev, "%s: miss cal spec : %d,%d,%d\n", __func__,
					wreg[0], wreg[1], wreg[2]);
		}
		ret = sec_ts_release_tmode(ts);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: i2c fail!, %d\n", __func__, ret);
			mis_cal_data = 0xF4;
			goto NG;
		}
	}

	snprintf(buff, sizeof(buff), "%d", mis_cal_data);
	snprintf(buff_all, sizeof(buff_all), "%d,%d,%d,%d", mis_cal_data, wreg[0], wreg[1], wreg[2]);

	sec_cmd_set_cmd_result(sec, buff_all, strnlen(buff_all, sizeof(buff_all)));
	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING)
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "MIS_CAL");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff_all);
	return;

NG:
	snprintf(buff, sizeof(buff), "%d", mis_cal_data);
	snprintf(buff_all, sizeof(buff_all), "%d,%d,%d,%d", mis_cal_data, 0, 0, 0);
	sec_cmd_set_cmd_result(sec, buff_all, strnlen(buff_all, sizeof(buff_all)));
	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING)
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "MIS_CAL");
	sec->cmd_state = SEC_CMD_STATUS_FAIL;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff_all);
}


static void get_wet_mode(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };
	char wet_mode_info = 0;
	int ret;

	sec_cmd_set_default_result(sec);

	sec_ts_delay(300);

	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_WET_MODE, &wet_mode_info, 1);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: i2c fail!, %d\n", __func__, ret);
		goto NG;
	}

	snprintf(buff, sizeof(buff), "%d", wet_mode_info);
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING)
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "WET_MODE");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
	return;

NG:
	snprintf(buff, sizeof(buff), "NG");
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING)
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "WET_MODE");
	sec->cmd_state = SEC_CMD_STATUS_FAIL;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);

}

static void get_x_num(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };

	sec_cmd_set_default_result(sec);
	snprintf(buff, sizeof(buff), "%d", ts->tx_count);
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void get_y_num(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };

	sec_cmd_set_default_result(sec);
	snprintf(buff, sizeof(buff), "%d", ts->rx_count);
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void get_x_cross_routing(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };

	sec_cmd_set_default_result(sec);
	snprintf(buff, sizeof(buff), "NG");
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void get_y_cross_routing(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };
	int ret;

	sec_cmd_set_default_result(sec);

	ret = strncmp(ts->plat_data->model_name, "G935", 4)
		&& strncmp(ts->plat_data->model_name, "N930", 4);
	if (ret == 0)
		snprintf(buff, sizeof(buff), "13,14");
	else
		snprintf(buff, sizeof(buff), "NG");
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static u32 checksum_sum(struct sec_ts_data *ts, u8 *chunk_data, u32 size)
{
	u32 data_32 = 0;
	u32 checksum = 0;
	u8 *fd = chunk_data;
	int i;

	for (i = 0; i < size/4; i++) {
		data_32 = (((fd[(i*4)+0]&0xFF)<<0) | ((fd[(i*4)+1]&0xFF)<<8) |
					((fd[(i*4)+2]&0xFF)<<16) | ((fd[(i*4)+3]&0xFF)<<24));
		checksum += data_32;
	}

	checksum &= 0xFFFFFFFF;

	input_info(true, &ts->client->dev, "%s: checksum = [%x]\n", __func__, checksum);

	return checksum;
}

static u32 get_bin_checksum(struct sec_ts_data *ts, const u8 *data, size_t size)
{

	int i;
	fw_header *fw_hd;
	fw_chunk *fw_ch;
	u8 *fd = (u8 *)data;
	u32 data_32;
	u32 checksum = 0;
	u32 chunk_checksum[4];
	u32 img_checksum;

	fw_hd = (fw_header *)fd;
	fd += sizeof(fw_header);

	if (fw_hd->signature != SEC_TS_FW_HEADER_SIGN) {
		input_err(true, &ts->client->dev, "%s: firmware header error = %08X\n", __func__, fw_hd->signature);
		return 0;
	}

	for (i = 0; i < (fw_hd->totalsize/4); i++) {
		data_32 = (((data[(i*4)+0]&0xFF)<<0) | ((data[(i*4)+1]&0xFF)<<8) |
					((data[(i*4)+2]&0xFF)<<16) | ((data[(i*4)+3]&0xFF)<<24));
		checksum += data_32;
	}

	input_info(true, &ts->client->dev, "%s: fw_hd->totalsize = [%d] checksum = [%x]\n",
				__func__, fw_hd->totalsize, checksum);

	checksum &= 0xFFFFFFFF;
	if (checksum != 0) {
		input_err(true, &ts->client->dev, "%s: Checksum fail! = %08X\n", __func__, checksum);
		return 0;
	}

	for (i = 0; i < fw_hd->num_chunk; i++) {
		fw_ch = (fw_chunk *)fd;

		input_info(true, &ts->client->dev, "%s: [%d] 0x%08X, 0x%08X, 0x%08X, 0x%08X\n", __func__, i,
				fw_ch->signature, fw_ch->addr, fw_ch->size, fw_ch->reserved);

		if (fw_ch->signature != SEC_TS_FW_CHUNK_SIGN) {
			input_err(true, &ts->client->dev, "%s: firmware chunk error = %08X\n",
						__func__, fw_ch->signature);
			return 0;
		}
		fd += sizeof(fw_chunk);

		checksum = checksum_sum(ts, fd, fw_ch->size);
		chunk_checksum[i] = checksum;
		fd += fw_ch->size;
	}

#ifdef CONFIG_DUAL_TSP
	input_err(true, &ts->client->dev, "ic fw ver %d, flip_status %d, checksum[0x%x, 0x%x, 0x%x, 0x%x]", 
		ts->plat_data->img_version_of_ic[3], ts->flip_status_current, chunk_checksum[0], chunk_checksum[1], chunk_checksum[2], chunk_checksum[3]);

	img_checksum = chunk_checksum[0] + ( ts->flip_status_current ? chunk_checksum[2]:chunk_checksum[1] );
#else
	img_checksum = chunk_checksum[0] + chunk_checksum[1];
#endif

	return img_checksum;
}

static void get_checksum_data(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };
	char csum_result[4] = { 0 };
	char data[5] = { 0 };
	u8 cal_result;
	u8 nv_result;
	u8 temp;
	u8 csum = 0;
	int ret, i;

	u32 ic_checksum;
	u32 img_checksum;
	const struct firmware *fw_entry;
	char fw_path[SEC_TS_MAX_FW_PATH];
	u8 fw_ver[4];

	sec_cmd_set_default_result(sec);
	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n", __func__);
		snprintf(buff, sizeof(buff), "%s", "TSP turned off");
		goto err;
	}

	disable_irq(ts->client->irq);

	ts->plat_data->power(ts, false);
	ts->power_status = SEC_TS_STATE_POWER_OFF;
	sec_ts_delay(50);

	ts->plat_data->power(ts, true);
	ts->power_status = SEC_TS_STATE_POWER_ON;
	sec_ts_delay(70);

	ret = sec_ts_wait_for_ready(ts, SEC_TS_ACK_BOOT_COMPLETE);
	if (ret < 0) {
		enable_irq(ts->client->irq);
		input_err(true, &ts->client->dev, "%s: boot complete failed\n", __func__);
		snprintf(buff, sizeof(buff), "%s", "NG");
		goto err;
	}

	ret = ts->sec_ts_i2c_read(ts, SEC_TS_READ_FIRMWARE_INTEGRITY, &data[0], 1);
	if (ret < 0 || (data[0] != 0x80)) {
		enable_irq(ts->client->irq);
		input_err(true, &ts->client->dev, "%s: firmware integrity failed, ret:%d, data:%X\n",
				__func__, ret, data[0]);
		snprintf(buff, sizeof(buff), "%s", "NG");
		goto err;
	}

	ret = ts->sec_ts_i2c_read(ts, SEC_TS_READ_BOOT_STATUS, &data[1], 1);
	if (ret < 0 || (data[1] != SEC_TS_STATUS_APP_MODE)) {
		enable_irq(ts->client->irq);
		input_err(true, &ts->client->dev, "%s: boot status failed, ret:%d, data:%X\n", __func__, ret, data[0]);
		snprintf(buff, sizeof(buff), "%s", "NG");
		goto err;
	}

	ret = ts->sec_ts_i2c_read(ts, SEC_TS_READ_TS_STATUS, &data[2], 4);
	if (ret < 0 || (data[3] == TOUCH_SYSTEM_MODE_FLASH)) {
		enable_irq(ts->client->irq);
		input_err(true, &ts->client->dev, "%s: touch status failed, ret:%d, data:%X\n", __func__, ret, data[3]);
		snprintf(buff, sizeof(buff), "%s", "NG");
		goto err;
	}

	sec_ts_reinit(ts);

	enable_irq(ts->client->irq);

	input_err(true, &ts->client->dev, "%s: data[0]:%X, data[1]:%X, data[3]:%X\n", __func__, data[0], data[1], data[3]);

	temp = DO_FW_CHECKSUM | DO_PARA_CHECKSUM;
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_GET_CHECKSUM, &temp, 1);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: send get_checksum_cmd fail!\n", __func__);
		snprintf(buff, sizeof(buff), "%s", "SendCMDfail");
		goto err;
	}

	sec_ts_delay(20);

	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_GET_CHECKSUM, csum_result, 4);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: read get_checksum result fail!\n", __func__);
		snprintf(buff, sizeof(buff), "%s", "ReadCSUMfail");
		goto err;
	}

	nv_result = get_tsp_nvm_data(ts, SEC_TS_NVM_OFFSET_FAC_RESULT);
	nv_result += get_tsp_nvm_data(ts, SEC_TS_NVM_OFFSET_CAL_COUNT);

	cal_result = sec_ts_read_calibration_report(ts);

	for (i = 0; i < 4; i++)
		csum += csum_result[i];

	csum += nv_result;
	csum += cal_result;

	csum = ~csum;

	if (ts->plat_data->firmware_name != NULL) {

		ret = ts->sec_ts_i2c_read(ts, SEC_TS_READ_IMG_VERSION, fw_ver, 4);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: firmware version read error\n", __func__);
		}

		for (i = 0; i < 4; i++) {
			if (ts->plat_data->img_version_of_bin[i] !=  fw_ver[i]) {
				input_err(true, &ts->client->dev, "%s: do not matched version info [%d]:[%x]!=[%x] and skip!\n",
							__func__, i, ts->plat_data->img_version_of_bin[i], fw_ver[i]);
				goto out;
			}
		}

		ic_checksum = (((csum_result[0]&0xFF)<<24) | ((csum_result[1]&0xFF)<<16) |
						((csum_result[2]&0xFF)<<8) | ((csum_result[3]&0xFF)<<0));

		snprintf(fw_path, SEC_TS_MAX_FW_PATH, "%s", ts->plat_data->firmware_name);

		if (request_firmware(&fw_entry, fw_path, &ts->client->dev) !=  0) {
			input_err(true, &ts->client->dev, "%s: firmware is not available\n", __func__);
			snprintf(buff, sizeof(buff), "%s", "NG");
			goto err;
		}

		img_checksum = get_bin_checksum(ts, fw_entry->data, fw_entry->size);

		release_firmware(fw_entry);

		input_info(true, &ts->client->dev, "%s: img_checksum=[0x%X], ic_checksum=[0x%X]\n",
						__func__, img_checksum, ic_checksum);

		if (img_checksum != ic_checksum) {
			input_err(true, &ts->client->dev, "%s: img_checksum=[0x%X] != ic_checksum=[0x%X]!!!\n",
							__func__, img_checksum, ic_checksum);
			snprintf(buff, sizeof(buff), "%s", "NG");
			goto err;
		}
	}
out:

	input_info(true, &ts->client->dev, "%s: checksum = %02X\n", __func__, csum);
	snprintf(buff, sizeof(buff), "%02X", csum);
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	return;

err:
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
}

static void run_reference_read(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_OFFSET_DATA_SEC;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void run_reference_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_OFFSET_DATA_SEC;
	mode.allnode = TEST_MODE_ALL_NODE;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void get_reference(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	short val = 0;
	int node = 0;

	sec_cmd_set_default_result(sec);
	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		snprintf(buff, sizeof(buff), "%s", "TSP turned off");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		return;
	}

	node = sec_ts_check_index(ts);
	if (node < 0)
		return;

	val = ts->pFrame[node];
	snprintf(buff, sizeof(buff), "%d", val);
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void run_rawcap_read(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_OFFSET_DATA_SDC;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void run_rawcap_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_OFFSET_DATA_SDC;
	mode.allnode = TEST_MODE_ALL_NODE;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void get_rawcap(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	short val = 0;
	int node = 0;

	sec_cmd_set_default_result(sec);
	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		snprintf(buff, sizeof(buff), "%s", "TSP turned off");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		return;
	}

	node = sec_ts_check_index(ts);
	if (node < 0)
		return;

	val = ts->pFrame[node];
	snprintf(buff, sizeof(buff), "%d", val);
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void run_delta_read(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_RAW_DATA;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void run_delta_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_RAW_DATA;
	mode.allnode = TEST_MODE_ALL_NODE;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void get_delta(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	short val = 0;
	int node = 0;

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		snprintf(buff, sizeof(buff), "%s", "TSP turned off");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		return;
	}

	node = sec_ts_check_index(ts);
	if (node < 0)
		return;

	val = ts->pFrame[node];
	snprintf(buff, sizeof(buff), "%d", val);
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void run_raw_p2p_read(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));

	/* both mutual, self for factory_cmd_result_all*/
	sec_ts_read_rawp2p_data_all(ts, sec, &mode);
}

static void get_p2p_read_one_cmd(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };

	sec_cmd_set_default_result(sec);

	input_info(true, &ts->client->dev, "%s: start\n", __func__);

	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING) {
		snprintf(buff, sizeof(buff), "%d,%d", ts->cm_raw_set_p2p_min, ts->cm_raw_set_p2p_max);
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "CM_RAW_SET_P2P");
		snprintf(buff, sizeof(buff), "%d,%d", 0, ts->cm_raw_set_p2p_diff);
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "CM_RAW_SET_P2P_DIFF");
		snprintf(buff, sizeof(buff), "%d,%d", ts->self_raw_set_p2p_x_min, ts->self_raw_set_p2p_x_max);
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "SELF_RAW_SET_P2P_X");
		snprintf(buff, sizeof(buff), "%d,%d", ts->self_raw_set_p2p_y_min, ts->self_raw_set_p2p_y_max);
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "SELF_RAW_SET_P2P_Y");
		snprintf(buff, sizeof(buff), "%d,%d", 0, ts->self_raw_set_p2p_x_diff);
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "SELF_RAW_SET_P2P_X_DIFF");
		snprintf(buff, sizeof(buff), "%d,%d", 0, ts->self_raw_set_p2p_y_diff);
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "SELF_RAW_SET_P2P_Y_DIFF");
	}
	sec->cmd_state = SEC_CMD_STATUS_OK;
}

static void run_raw_p2p_min_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_RAW_DATA_P2P_MIN;
	mode.allnode = TEST_MODE_ALL_NODE;

	sec_ts_read_rawp2p_data(ts, sec, &mode);
}

static void run_raw_p2p_max_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_RAW_DATA_P2P_MAX;
	mode.allnode = TEST_MODE_ALL_NODE;

	sec_ts_read_rawp2p_data(ts, sec, &mode);
}

static int run_raw_self_p2p_diff_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	short max = 0, tmin, tmax;
	int i, ret = 0;
	char temp[SEC_CMD_STR_LEN] = { 0 };
	char *buff;
	char para = TO_TOUCH_MODE;

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n", __func__);
		goto error_power_state;	
	}

	buff = kzalloc(ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN, GFP_KERNEL);
	if (!buff)
		goto error_alloc_mem;

	disable_irq(ts->client->irq);

	ret = sec_ts_p2p_tmode(ts);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: failed to fix p2p tmode\n", __func__);
		goto error_tmode_fail;
	}

	ret = execute_p2ptest(ts);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: failed to fix p2p test\n", __func__);
		goto error_test_fail;
	}

	// cal min frame
	ret = sec_ts_read_frame(ts, TYPE_RAW_DATA_P2P_MIN, &tmin, &tmax, true);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: failed to read frame\n",
				__func__);
		goto error_test_fail;
	}

	// cal max frame
	ret = sec_ts_read_frame(ts, TYPE_RAW_DATA_P2P_MAX, &tmin, &tmax, true);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: failed to read frame\n",
				__func__);
		goto error_test_fail;
	}

	for (i = 0; i < (ts->rx_count * ts->tx_count); i++) {
		snprintf(temp, CMD_RESULT_WORD_LEN, "%d,", ts->pmaxFrame[i] - ts->pminFrame[i]);
		strncat(buff, temp, CMD_RESULT_WORD_LEN);
	
		memset(temp, 0x00, SEC_CMD_STR_LEN);
	}

	// cal diff value
	for (i = 0; i < ts->tx_count * ts->rx_count; i++) {
		printk("HQJ test: %d-%d > %d\n", ts->pmaxFrame[i], ts->pminFrame[i], max);
		if(ts->pmaxFrame[i] - ts->pminFrame[i] > max)
			max = ts->pmaxFrame[i] - ts->pminFrame[i];
	}
	ts->cm_raw_set_p2p_diff = max;

	// cal min frame
	ret = sec_ts_read_channel(ts, TYPE_RAW_DATA_P2P_MIN, &tmin, &tmax, true);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: failed to read frame\n", __func__);
		goto error_test_fail;
	}

	ret = sec_ts_read_channel(ts, TYPE_RAW_DATA_P2P_MAX, &tmin, &tmax, true);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: failed to read frame\n", __func__);
		goto error_test_fail;
	}

	// cal diff value
	for (i = 0; i < ts->tx_count; i++) {
		printk("HQJ test: %d-%d > %d\n", ts->pmaxFrame[i], ts->pminFrame[i], max);
		if(ts->pmaxFrame[i] - ts->pminFrame[i] > max)
			max = ts->pmaxFrame[i] - ts->pminFrame[i];
	}
	ts->self_raw_set_p2p_y_diff = max;
	max = 0;
	for (i = ts->tx_count; i < ts->rx_count + ts->tx_count; i++) {
		printk("HQJ test: %d-%d > %d\n", ts->pmaxFrame[i], ts->pminFrame[i], max);
		if(ts->pmaxFrame[i] - ts->pminFrame[i] > max)
			max = ts->pmaxFrame[i] - ts->pminFrame[i];
	}
	
	ts->self_raw_set_p2p_x_diff = max;

	if (!sec)
		goto out_rawdata;

	sec_ts_locked_release_all_finger(ts);
	sec_ts_delay(30);
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, &para, 1);
	if (ret < 0)
		input_err(true, &ts->client->dev, "%s: set rawdata type failed!\n", __func__);

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN));
	sec->cmd_state = SEC_CMD_STATUS_OK;

out_rawdata:
	enable_irq(ts->client->irq);
	kfree(buff);

	return ret;
error_test_fail:
error_tmode_fail:
	kfree(buff);
	enable_irq(ts->client->irq);
error_alloc_mem:
error_power_state:
	if (!sec)
		return ret;

	snprintf(temp, SEC_CMD_STR_LEN, "FAIL");
	sec_cmd_set_cmd_result(sec, temp, SEC_CMD_STR_LEN);
	sec->cmd_state = SEC_CMD_STATUS_FAIL;

	return ret;
}

/* self reference : send TX power in TX channel, receive in TX channel */
static void run_self_reference_read(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_OFFSET_DATA_SEC;
	mode.frame_channel = TEST_MODE_READ_CHANNEL;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void run_self_reference_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_OFFSET_DATA_SEC;
	mode.frame_channel = TEST_MODE_READ_CHANNEL;
	mode.allnode = TEST_MODE_ALL_NODE;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void run_self_rawcap_read(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_OFFSET_DATA_SDC;
	mode.frame_channel = TEST_MODE_READ_CHANNEL;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void run_self_rawcap_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_OFFSET_DATA_SDC;
	mode.frame_channel = TEST_MODE_READ_CHANNEL;
	mode.allnode = TEST_MODE_ALL_NODE;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void run_self_delta_read(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_RAW_DATA;
	mode.frame_channel = TEST_MODE_READ_CHANNEL;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void run_self_delta_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_RAW_DATA;
	mode.frame_channel = TEST_MODE_READ_CHANNEL;
	mode.allnode = TEST_MODE_ALL_NODE;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void run_self_raw_p2p_min_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_RAW_DATA_P2P_MIN;
	mode.frame_channel = TEST_MODE_READ_CHANNEL;
	mode.allnode = TEST_MODE_ALL_NODE;

	sec_ts_read_rawp2p_data(ts, sec, &mode);
}

static void run_self_raw_p2p_max_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_RAW_DATA_P2P_MAX;
	mode.frame_channel = TEST_MODE_READ_CHANNEL;
	mode.allnode = TEST_MODE_ALL_NODE;

	sec_ts_read_rawp2p_data(ts, sec, &mode);
}

void sec_ts_get_saved_cmoffset(struct sec_ts_data *ts)
{
	int rc;
	u8 *rBuff = NULL;
	int i;
	int result_size = SEC_TS_SELFTEST_REPORT_SIZE + ts->tx_count * ts->rx_count * 2;
	short min, max;

	input_raw_info(true, &ts->client->dev, "%s:\n", __func__);

	rBuff = kzalloc(result_size, GFP_KERNEL);
	if (!rBuff)
		return;

	rc = ts->sec_ts_i2c_read(ts, SEC_TS_READ_SELFTEST_RESULT, rBuff, result_size);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: Selftest execution time out!\n", __func__);
		goto err_exit;
	}

	memset(ts->pFrame, 0x00, result_size - SEC_TS_SELFTEST_REPORT_SIZE);

	for (i = 0; i < result_size - SEC_TS_SELFTEST_REPORT_SIZE; i += 2)
		ts->pFrame[i / 2] = (rBuff[SEC_TS_SELFTEST_REPORT_SIZE + i + 1] << 8)
				| rBuff[SEC_TS_SELFTEST_REPORT_SIZE + i];

	min = max = ts->pFrame[0];

	sec_ts_print_frame(ts, &min, &max);

err_exit:
	kfree(rBuff);
}

/*
 * sec_ts_run_rawdata_all : read all raw data
 *
 * when you want to read full raw data (full_read : true)
 * "mutual/self 3, 5, 29, 1, 19" data will be saved in log
 *
 * otherwise, (full_read : false, especially on boot time)
 * only "mutual 3, 5, 29" data will be saved in log
 */
void sec_ts_run_rawdata_all(struct sec_ts_data *ts, bool full_read)
{
	short min, max;
	int ret, i, read_num;
	u8 test_type[5] = {TYPE_AMBIENT_DATA, TYPE_DECODED_DATA,
		TYPE_SIGNAL_DATA, TYPE_OFFSET_DATA_SEC, TYPE_OFFSET_DATA_SDC};
#if 0//def USE_PRESSURE_SENSOR
	short pressure[3] = { 0 };
	u8 cal_data[18] = { 0 };
#endif

	ts->tsp_dump_lock = 1;
	input_raw_data_clear();
	input_raw_info(true, &ts->client->dev,
			"%s: start (noise:%d, wet:%d)##\n",
			__func__, ts->touch_noise_status, ts->wet_mode);

	ret = sec_ts_fix_tmode(ts, TOUCH_SYSTEM_MODE_TOUCH, TOUCH_MODE_STATE_TOUCH);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: failed to fix tmode\n",
				__func__);
		goto out;
	}

	if (full_read) {
		read_num = 5;
	} else {
		read_num = 3;
		test_type[read_num - 1] = TYPE_OFFSET_DATA_SDC;
	}

	for (i = 0; i < read_num; i++) {
		ret = sec_ts_read_frame(ts, test_type[i], &min, &max, false);
		if (ret < 0)
			input_raw_info(true, &ts->client->dev,
					"%s: mutual %d : error ## ret:%d\n",
					__func__, test_type[i], ret);
		else
			input_raw_info(true, &ts->client->dev,
					"%s: mutual %d : Max/Min %d,%d ##\n",
					__func__, test_type[i], max, min);
		sec_ts_delay(20);

#ifdef MINORITY_REPORT
		if (test_type[i] == TYPE_AMBIENT_DATA) {
			minority_report_calculate_rawdata(ts);
		} else if (test_type[i] == TYPE_OFFSET_DATA_SDC) {
			minority_report_calculate_ito(ts);
			minority_report_sync_latest_value(ts);
		}
#endif
		if (full_read) {
			ret = sec_ts_read_channel(ts, test_type[i], &min, &max, false);
			if (ret < 0)
				input_raw_info(true, &ts->client->dev,
						"%s: self %d : error ## ret:%d\n",
						__func__, test_type[i], ret);
			else
				input_raw_info(true, &ts->client->dev,
						"%s: self %d : Max/Min %d,%d ##\n",
						__func__, test_type[i], max, min);
			sec_ts_delay(20);
		}
	}

#if 0//def USE_PRESSURE_SENSOR
	ret = sec_ts_fix_tmode(ts, TOUCH_SYSTEM_MODE_TOUCH, TOUCH_MODE_STATE_TOUCH);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: failed to fix tmode\n",
				__func__);
		goto out;
	}

	/* run pressure offset data read */
	read_pressure_data(ts, TYPE_OFFSET_DATA_SEC, pressure);
	sec_ts_delay(20);

	/* run pressure rawdata read */
	read_pressure_data(ts, TYPE_RAW_DATA, pressure);
	sec_ts_delay(20);

	/* run pressure raw delta read  */
	read_pressure_data(ts, TYPE_REMV_AMB_DATA, pressure);
	sec_ts_delay(20);

	/* run pressure sigdata read */
	read_pressure_data(ts, TYPE_SIGNAL_DATA, pressure);
	sec_ts_delay(20);

	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_SET_GET_PRESSURE, cal_data, 18);
	ts->pressure_left = ((cal_data[16] << 8) | cal_data[17]);
	ts->pressure_center = ((cal_data[8] << 8) | cal_data[9]);
	ts->pressure_right = ((cal_data[0] << 8) | cal_data[1]);
	input_raw_info(true, &ts->client->dev, "%s: pressure cal data - Left: %d, Center: %d, Right: %d\n",
			__func__, ts->pressure_left, ts->pressure_center,
			ts->pressure_right);
#endif
	sec_ts_release_tmode(ts);

out:
	input_raw_info(true, &ts->client->dev, "%s: ito : %02X %02X %02X %02X\n",
			__func__, ts->ito_test[0], ts->ito_test[1]
			, ts->ito_test[2], ts->ito_test[3]);

	input_raw_info(true, &ts->client->dev, "%s: done (noise:%d, wet:%d)##\n",
			__func__, ts->touch_noise_status, ts->wet_mode);
	ts->tsp_dump_lock = 0;

	sec_ts_locked_release_all_finger(ts);
}

static void run_rawdata_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };

	sec_cmd_set_default_result(sec);

	if (ts->tsp_dump_lock == 1) {
		input_err(true, &ts->client->dev, "%s: already checking now\n", __func__);
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		goto out;
	}
	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: IC is power off\n", __func__);
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		goto out;
	}

	sec_ts_run_rawdata_all(ts, true);

	snprintf(buff, sizeof(buff), "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;
out:
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

/* Use TSP NV area
 * buff[0] : offset from user NVM storage
 * buff[1] : length of stroed data - 1 (ex. using 1byte, value is  1 - 1 = 0)
 * buff[2] : write data
 * buff[..] : cont.
 */
void set_tsp_nvm_data_clear(struct sec_ts_data *ts, u8 offset)
{
	char buff[4] = { 0 };
	int ret;

	input_info(true, &ts->client->dev, "%s\n", __func__);

	buff[0] = offset;

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_NVM, buff, 3);
	if (ret < 0)
		input_err(true, &ts->client->dev, "%s: nvm write failed. ret: %d\n", __func__, ret);

	sec_ts_delay(20);
}

int get_tsp_nvm_data(struct sec_ts_data *ts, u8 offset)
{
	char buff[2] = { 0 };
	int ret;

	/* SENSE OFF -> CELAR EVENT STACK -> READ NV -> SENSE ON */
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SENSE_OFF, NULL, 0);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: fail to write Sense_off\n", __func__);
		goto out_nvm;
	}

	input_dbg(false, &ts->client->dev, "%s: SENSE OFF\n", __func__);

	sec_ts_delay(100);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_CLEAR_EVENT_STACK, NULL, 0);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: i2c write clear event failed\n", __func__);
		goto out_nvm;
	}

	input_dbg(false, &ts->client->dev, "%s: CLEAR EVENT STACK\n", __func__);

	sec_ts_delay(100);

	sec_ts_locked_release_all_finger(ts);

	/* send NV data using command
	 * Use TSP NV area : in this model, use only one byte
	 * buff[0] : offset from user NVM storage
	 * buff[1] : length of stroed data - 1 (ex. using 1byte, value is  1 - 1 = 0)
	 */
	memset(buff, 0x00, 2);
	buff[0] = offset;
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_NVM, buff, 2);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: nvm send command failed. ret: %d\n", __func__, ret);
		goto out_nvm;
	}

	sec_ts_delay(20);

	/* read NV data
	 * Use TSP NV area : in this model, use only one byte
	 */
	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_NVM, buff, 1);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: nvm send command failed. ret: %d\n", __func__, ret);
		goto out_nvm;
	}

	input_info(true, &ts->client->dev, "%s: offset:%u  data:%02X\n", __func__,offset, buff[0]);

out_nvm:
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SENSE_ON, NULL, 0);
	if (ret < 0)
		input_err(true, &ts->client->dev, "%s: fail to write Sense_on\n", __func__);

	input_dbg(false, &ts->client->dev, "%s: SENSE ON\n", __func__);

	return buff[0];
}

int get_tsp_nvm_data_by_size(struct sec_ts_data *ts, u8 offset, int length, u8 *data)
{
	char *buff = NULL;
	int ret;

	buff = kzalloc(length, GFP_KERNEL);
	if (!buff)
		return -ENOMEM;

	input_info(true, &ts->client->dev, "%s: offset:%u, length:%d, size:%d\n", __func__, offset, length, sizeof(data));

	/* SENSE OFF -> CELAR EVENT STACK -> READ NV -> SENSE ON */
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SENSE_OFF, NULL, 0);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: fail to write Sense_off\n", __func__);
		goto out_nvm;
	}

	input_dbg(true, &ts->client->dev, "%s: SENSE OFF\n", __func__);

	sec_ts_delay(100);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_CLEAR_EVENT_STACK, NULL, 0);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: i2c write clear event failed\n", __func__);
		goto out_nvm;
	}

	input_dbg(true, &ts->client->dev, "%s: CLEAR EVENT STACK\n", __func__);

	sec_ts_delay(100);

	sec_ts_locked_release_all_finger(ts);

	/* send NV data using command
	 * Use TSP NV area : in this model, use only one byte
	 * buff[0] : offset from user NVM storage
	 * buff[1] : length of stroed data - 1 (ex. using 1byte, value is  1 - 1 = 0)
	 */
	memset(buff, 0x00, 2);
	buff[0] = offset;
	buff[1] = length - 1;
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_NVM, buff, 2);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: nvm send command failed. ret: %d\n", __func__, ret);
		goto out_nvm;
	}

	sec_ts_delay(20);

	/* read NV data
	 * Use TSP NV area : in this model, use only one byte
	 */
	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_NVM, buff, length);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: nvm send command failed. ret: %d\n", __func__, ret);
		goto out_nvm;
	}

	memcpy(data, buff, length);

out_nvm:
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SENSE_ON, NULL, 0);
	if (ret < 0)
		input_err(true, &ts->client->dev, "%s: fail to write Sense_on\n", __func__);

	input_dbg(true, &ts->client->dev, "%s: SENSE ON\n", __func__);

	kfree(buff);

	return ret;
}

#ifdef TCLM_CONCEPT
int sec_tclm_data_read(struct i2c_client *client, int address)
{
	struct sec_ts_data *ts = i2c_get_clientdata(client);
	int ret = 0;
	u8 reg = 0;
	u8 buff[4];

	bool tune_ver_flag = false;

	switch (address) {
	case SEC_TCLM_NVM_OFFSET_FAC_RESULT:
		reg = SEC_TS_NVM_OFFSET_FAC_RESULT;
		break;
	case SEC_TCLM_NVM_OFFSET_CAL_COUNT:
		reg = SEC_TS_NVM_OFFSET_CAL_COUNT;
		break;
	case SEC_TCLM_NVM_OFFSET_TUNE_VERSION:
		tune_ver_flag = true;
		break;
	case SEC_TCLM_NVM_OFFSET_CAL_POSITION:
		reg = SEC_TS_NVM_OFFSET_CAL_POSITION;
		break;
	case SEC_TCLM_NVM_OFFSET_HISTORY_QUEUE_COUNT:
		reg = SEC_TS_NVM_OFFSET_HISTORY_QUEUE_COUNT;
		break;
	case SEC_TCLM_NVM_OFFSET_HISTORY_QUEUE_LASTP:
		reg = SEC_TS_NVM_OFFSET_HISTORY_QUEUE_LASTP;
		break;
	case SEC_TCLM_NVM_OFFSET_HISTORY_QUEUE_ZERO:
		reg = SEC_TS_NVM_OFFSET_HISTORY_QUEUE_ZERO;
		break;
	case SEC_TCLM_NVM_OFFSET_HISTORY_QUEUE_SIZE:
		ret = get_tsp_nvm_data_by_size(ts, SEC_TS_NVM_OFFSET_HISTORY_QUEUE_ZERO,
				ts->tdata->cal_pos_hist_cnt * 2, ts->tdata->cal_pos_hist_queue);
		return ret;
	case SEC_TCLM_NVM_OFFSET_IC_FIRMWARE_VER:
		ts->sec_ts_i2c_read(ts, SEC_TS_READ_IMG_VERSION, buff, 4);
		ret =  buff[2] << 8 | buff[3];
		return ret;
	default:
		return 0;
	}

	if (tune_ver_flag) {
		ret = get_tsp_nvm_data(ts, SEC_TS_NVM_OFFSET_TUNE_VERSION + 1);
		return ret;
	}
	ret = get_tsp_nvm_data(ts, reg);

	return ret;
}

void sec_tclm_data_write(struct i2c_client *client, int address, int data)
{
	struct sec_ts_data *ts = i2c_get_clientdata(client);
	u8 reg = 0;
	bool tune_ver_flag = false;

	switch (address) {
	case SEC_TCLM_NVM_OFFSET_FAC_RESULT:
		reg = SEC_TS_NVM_OFFSET_FAC_RESULT;
		break;
	case SEC_TCLM_NVM_OFFSET_CAL_COUNT:
		reg = SEC_TS_NVM_OFFSET_CAL_COUNT;
		break;
	case SEC_TCLM_NVM_OFFSET_TUNE_VERSION:
		reg = SEC_TS_NVM_OFFSET_TUNE_VERSION;
		tune_ver_flag = true;
		break;
	case SEC_TCLM_NVM_OFFSET_CAL_POSITION:
		reg = SEC_TS_NVM_OFFSET_CAL_POSITION;
		break;
	case SEC_TCLM_NVM_OFFSET_HISTORY_QUEUE_COUNT:
		reg = SEC_TS_NVM_OFFSET_HISTORY_QUEUE_COUNT;
		break;
	case SEC_TCLM_NVM_OFFSET_HISTORY_QUEUE_LASTP:
		reg = SEC_TS_NVM_OFFSET_HISTORY_QUEUE_LASTP;
		break;
	case SEC_TCLM_NVM_OFFSET_HISTORY_QUEUE_ZERO:
		reg = SEC_TS_NVM_OFFSET_HISTORY_QUEUE_ZERO;
		break;
	case SEC_TCLM_NVM_OFFSET_HISTORY_QUEUE_SAVE:
		sec_ts_tclm_set_nvm_data(ts, SEC_TS_NVM_OFFSET_HISTORY_QUEUE_ZERO + ts->tdata->cal_pos_hist_lastp * 2, ts->tdata->cal_position);
		sec_ts_tclm_set_nvm_data(ts, SEC_TS_NVM_OFFSET_HISTORY_QUEUE_ZERO + ts->tdata->cal_pos_hist_lastp * 2 + 1, ts->tdata->cal_count);
		return;
	default:
		return;
	}

	if (tune_ver_flag) {
		sec_ts_tclm_set_nvm_data(ts, reg, (u8)(data >> 8));
		sec_ts_tclm_set_nvm_data(ts, reg + 1, (u8)(0xff & data));
	} else {
		sec_ts_tclm_set_nvm_data(ts, reg, (u8)data);
	}
}
void sec_ts_tclm_set_nvm_data(struct sec_ts_data *ts, u8 reg, u8 data)
{
	char buff[SEC_CMD_STR_LEN] = {0};
	int rc;

	buff[0] = reg;
	buff[1] = 0;	/* 1bytes */
	buff[2] = (u8) data;
	input_info(true, &ts->client->dev, "%s: write (%d) to nvm register(%d)\n",
				__func__, buff[2], buff[0]);

	rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_NVM, buff, 3);
	if (rc < 0) {
		input_err(true, &ts->client->dev,
			"%s: nvm write failed. ret: %d\n", __func__, rc);
	}
	sec_ts_delay(20);
}

#endif

#ifdef MINORITY_REPORT

/*	ts->defect_probability is FFFFF
 *
 *	0	is 100% normal.
 *	1~9	is normal, but need check
 *	A~E	is abnormal, must check
 *	F	is Device defect
 *
 *	F----	: ito
 *	-F---	: rawdata
 *	--A--	: crc
 *	---A-	: i2c_err
 *	----A	: wet
 */
void minority_report_calculate_rawdata(struct sec_ts_data *ts)
{
	int ii;
	int temp = 0;
	int max = -30000;
	int min = 30000;
	int node_gap = 1;

	if (!ts->tx_count) {
		ts->item_rawdata = 0xD;
		return;
	}

	for (ii = 0; ii < (ts->rx_count * ts->tx_count); ii++) {
		if (max < ts->pFrame[ii]) {
			ts->max_ambient = max = ts->pFrame[ii];
			ts->max_ambient_channel_tx = ii % ts->tx_count;
			ts->max_ambient_channel_rx = ii / ts->tx_count;
		}
		if (min > ts->pFrame[ii]) {
			ts->min_ambient = min = ts->pFrame[ii];
			ts->min_ambient_channel_tx = ii % ts->tx_count;
			ts->min_ambient_channel_rx = ii / ts->tx_count;
		}

		if ((ii + 1) % (ts->tx_count) != 0) {
			temp = ts->pFrame[ii] - ts->pFrame[ii+1];
			if (temp < 0)
				temp = -temp;
			if (temp > node_gap)
				node_gap = temp;
		}

		if (ii < (ts->rx_count - 1) * ts->tx_count) {
			temp = ts->pFrame[ii] - ts->pFrame[ii + ts->tx_count];
			if (temp < 0)
				temp = -temp;
			if (temp > node_gap)
				node_gap = temp;
		}
	}

	if (max >= 80 || min <= -80)
		ts->item_rawdata = 0xF;
	else if ((max >= 50 || min <= -50) && (node_gap > 40))
		ts->item_rawdata = 0xC;
	else if (max >= 50 || min <= -50)
		ts->item_rawdata = 0xB;
	else if (node_gap > 40)
		ts->item_rawdata = 0xA;
	else if ((max >= 40 || min <= -40) && (node_gap > 30))
		ts->item_rawdata = 0x3;
	else if (max >= 40 || min <= -40)
		ts->item_rawdata = 0x2;
	else if (node_gap > 30)
		ts->item_rawdata = 0x1;
	else
		ts->item_rawdata = 0;

	input_info(true, &ts->client->dev, "%s min:%d,max:%d,node gap:%d =>%X\n",
			__func__, min, max, node_gap, ts->item_rawdata);

}

void minority_report_calculate_ito(struct sec_ts_data *ts)
{

	if (ts->ito_test[0] ||  ts->ito_test[1] || ts->ito_test[2] || ts->ito_test[3])
		ts->item_ito = 0xF;
	else
		ts->item_ito = 0;
}

u8 minority_report_check_count(int value)
{
	u8 ret;

	if (value > 160)
		ret = 0xA;
	else if (value > 90)
		ret = 3;
	else if (value > 40)
		ret = 2;
	else if (value > 10)
		ret = 1;
	else
		ret = 0;

	return ret;
}

void minority_report_sync_latest_value(struct sec_ts_data *ts)
{
	u32 temp = 0;

	/* crc */
	if (ts->checksum_result == 1)
		ts->item_crc = 0xA;

	/* i2c_err */
	ts->item_i2c_err = minority_report_check_count(ts->comm_err_count);

	/* wet */
	ts->item_wet = minority_report_check_count(ts->wet_count);

	temp |= (ts->item_ito & 0xF) << 16;
	temp |= (ts->item_rawdata & 0xF) << 12;
	temp |= (ts->item_crc & 0xF) << 8;
	temp |= (ts->item_i2c_err & 0xF) << 4;
	temp |= (ts->item_wet & 0xF);

	ts->defect_probability = temp;
}
#endif

/* FACTORY TEST RESULT SAVING FUNCTION
 * bit 3 ~ 0 : OCTA Assy
 * bit 7 ~ 4 : OCTA module
 * param[0] : OCTA modue(1) / OCTA Assy(2)
 * param[1] : TEST NONE(0) / TEST FAIL(1) / TEST PASS(2) : 2 bit
 */

#define TEST_OCTA_MODULE	1
#define TEST_OCTA_ASSAY		2

#define TEST_OCTA_NONE		0
#define TEST_OCTA_FAIL		1
#define TEST_OCTA_PASS		2

static void set_tsp_test_result(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_result *result;
	char buff[SEC_CMD_STR_LEN] = { 0 };
	char r_data[1] = { 0 };
	int ret = 0;

	input_info(true, &ts->client->dev, "%s\n", __func__);

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		snprintf(buff, sizeof(buff), "%s", "TSP_truned off");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		return;
	}

	r_data[0] = get_tsp_nvm_data(ts, SEC_TS_NVM_OFFSET_FAC_RESULT);
	if (r_data[0] == 0xFF)
		r_data[0] = 0;

	result = (struct sec_ts_test_result *)r_data;

	if (sec->cmd_param[0] == TEST_OCTA_ASSAY) {
		result->assy_result = sec->cmd_param[1];
		if (result->assy_count < 3)
			result->assy_count++;
	}

	if (sec->cmd_param[0] == TEST_OCTA_MODULE) {
		result->module_result = sec->cmd_param[1];
		if (result->module_count < 3)
			result->module_count++;
	}

	input_info(true, &ts->client->dev, "%s: %d, %d, %d, %d, 0x%X\n", __func__,
			result->module_result, result->module_count,
			result->assy_result, result->assy_count, result->data[0]);

	/* Use TSP NV area : in this model, use only one byte
	 * buff[0] : offset from user NVM storage
	 * buff[1] : length of stroed data - 1 (ex. using 1byte, value is  1 - 1 = 0)
	 * buff[2] : write data
	 */
	memset(buff, 0x00, SEC_CMD_STR_LEN);
	buff[2] = *result->data;

	input_info(true, &ts->client->dev, "%s: command (1)%X, (2)%X: %X\n",
			__func__, sec->cmd_param[0], sec->cmd_param[1], buff[2]);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_NVM, buff, 3);
	if (ret < 0)
		input_err(true, &ts->client->dev, "%s: nvm write failed. ret: %d\n", __func__, ret);

	sec_ts_delay(20);

	ts->nv = get_tsp_nvm_data(ts, SEC_TS_NVM_OFFSET_FAC_RESULT);

	snprintf(buff, sizeof(buff), "OK");
	sec_cmd_set_cmd_result(sec, buff, strlen(buff));
	sec->cmd_state = SEC_CMD_STATUS_OK;
}

static void get_tsp_test_result(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	struct sec_ts_test_result *result;

	input_info(true, &ts->client->dev, "%s\n", __func__);

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		snprintf(buff, sizeof(buff), "%s", "TSP_truned off");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		return;
	}

	memset(buff, 0x00, SEC_CMD_STR_LEN);
	buff[0] = get_tsp_nvm_data(ts, SEC_TS_NVM_OFFSET_FAC_RESULT);
	if (buff[0] == 0xFF) {
		set_tsp_nvm_data_clear(ts, SEC_TS_NVM_OFFSET_FAC_RESULT);
		buff[0] = 0;
	}

	ts->nv = buff[0];

	result = (struct sec_ts_test_result *)buff;

	input_info(true, &ts->client->dev, "%s: [0x%X][0x%X] M:%d, M:%d, A:%d, A:%d\n",
			__func__, *result->data, buff[0],
			result->module_result, result->module_count,
			result->assy_result, result->assy_count);

	snprintf(buff, sizeof(buff), "M:%s, M:%d, A:%s, A:%d",
			result->module_result == 0 ? "NONE" :
			result->module_result == 1 ? "FAIL" :
			result->module_result == 2 ? "PASS" : "A",
			result->module_count,
			result->assy_result == 0 ? "NONE" :
			result->assy_result == 1 ? "FAIL" :
			result->assy_result == 2 ? "PASS" : "A",
			result->assy_count);

	sec_cmd_set_cmd_result(sec, buff, strlen(buff));
	sec->cmd_state = SEC_CMD_STATUS_OK;
}

static void clear_tsp_test_result(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	int ret = 0;

	input_info(true, &ts->client->dev, "%s\n", __func__);

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		snprintf(buff, sizeof(buff), "%s", "TSP_truned off");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		return;
	}

	/* Use TSP NV area : in this model, use only one byte
	 * buff[0] : offset from user NVM storage
	 * buff[1] : length of stroed data - 1 (ex. using 1byte, value is  1 - 1 = 0)
	 * buff[2] : write data
	 */
	memset(buff, 0x00, SEC_CMD_STR_LEN);
	buff[0] = SEC_TS_NVM_OFFSET_FAC_RESULT;
	buff[1] = 0x00;
	buff[2] = 0x00;

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_NVM, buff, 3);
	if (ret < 0)
		input_err(true, &ts->client->dev, "%s: nvm write failed. ret: %d\n", __func__, ret);

	sec_ts_delay(20);

	ts->nv = get_tsp_nvm_data(ts, SEC_TS_NVM_OFFSET_FAC_RESULT);

	snprintf(buff, sizeof(buff), "OK");
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
}

static void increase_disassemble_count(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[3] = { 0 };
	int ret = 0;

	input_info(true, &ts->client->dev, "%s\n", __func__);

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		snprintf(buff, sizeof(buff), "%s", "TSP_truned off");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		return;
	}

	buff[2] = get_tsp_nvm_data(ts, SEC_TS_NVM_OFFSET_DISASSEMBLE_COUNT);

	input_info(true, &ts->client->dev, "%s: disassemble count is #1 %d\n", __func__, buff[2]);

	if (buff[2] == 0xFF)
		buff[2] = 0;

	if (buff[2] < 0xFE)
		buff[2]++;

	/* Use TSP NV area : in this model, use only one byte
	 * buff[0] : offset from user NVM storage
	 * buff[1] : length of stroed data - 1 (ex. using 1byte, value is  1 - 1 = 0)
	 * buff[2] : write data
	 */
	buff[0] = SEC_TS_NVM_OFFSET_DISASSEMBLE_COUNT;
	buff[1] = 0;

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_NVM, buff, 3);
	if (ret < 0)
		input_err(true, &ts->client->dev, "%s: nvm write failed. ret: %d\n", __func__, ret);

	sec_ts_delay(20);

	memset(buff, 0x00, 3);
	buff[0] = get_tsp_nvm_data(ts, SEC_TS_NVM_OFFSET_DISASSEMBLE_COUNT);
	input_info(true, &ts->client->dev, "%s: check disassemble count: %d\n", __func__, buff[0]);

	snprintf(buff, sizeof(buff), "OK");
	sec_cmd_set_cmd_result(sec, buff, strlen(buff));
	sec->cmd_state = SEC_CMD_STATUS_OK;
}

static void get_disassemble_count(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };

	input_info(true, &ts->client->dev, "%s\n", __func__);

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		snprintf(buff, sizeof(buff), "%s", "TSP_truned off");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		return;
	}

	memset(buff, 0x00, SEC_CMD_STR_LEN);
	buff[0] = get_tsp_nvm_data(ts, SEC_TS_NVM_OFFSET_DISASSEMBLE_COUNT);
	if (buff[0] == 0xFF) {
		set_tsp_nvm_data_clear(ts, SEC_TS_NVM_OFFSET_DISASSEMBLE_COUNT);
		buff[0] = 0;
	}

	input_info(true, &ts->client->dev, "%s: read disassemble count: %d\n", __func__, buff[0]);

	snprintf(buff, sizeof(buff), "%d", buff[0]);

	sec_cmd_set_cmd_result(sec, buff, strlen(buff));
	sec->cmd_state = SEC_CMD_STATUS_OK;
}

#define GLOVE_MODE_EN		(1 << 0)
#define CLEAR_COVER_EN		(1 << 1)
#define FAST_GLOVE_MODE_EN	(1 << 2)

static void glove_mode(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	int glove_mode_enables = 0;

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1) {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
	} else {
		int retval;

		if (sec->cmd_param[0])
			glove_mode_enables |= GLOVE_MODE_EN;
		else
			glove_mode_enables &= ~(GLOVE_MODE_EN);

		retval = sec_ts_glove_mode_enables(ts, glove_mode_enables);

		if (retval < 0) {
			input_err(true, &ts->client->dev, "%s: failed, retval = %d\n", __func__, retval);
			snprintf(buff, sizeof(buff), "NG");
			sec->cmd_state = SEC_CMD_STATUS_FAIL;
		} else {
			snprintf(buff, sizeof(buff), "OK");
			sec->cmd_state = SEC_CMD_STATUS_OK;
		}
	}

	sec_cmd_set_cmd_result(sec, buff, strlen(buff));
	sec->cmd_state = SEC_CMD_STATUS_WAITING;
	sec_cmd_set_cmd_exit(sec);
}

static void clear_cover_mode(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };

	input_info(true, &ts->client->dev, "%s: start clear_cover_mode %s\n", __func__, buff);
	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 3) {
		snprintf(buff, sizeof(buff), "%s", "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
	} else {
		if (sec->cmd_param[0] > 1) {
			ts->flip_enable = true;
			ts->cover_type = sec->cmd_param[1];
			ts->cover_cmd = (u8)ts->cover_type;
#ifdef CONFIG_TRUSTONIC_TRUSTED_UI
			if (TRUSTEDUI_MODE_TUI_SESSION & trustedui_get_current_mode()) {
				sec_ts_delay(500);
				tui_force_close(1);
				sec_ts_delay(200);
				if (TRUSTEDUI_MODE_TUI_SESSION & trustedui_get_current_mode()) {
					trustedui_clear_mask(TRUSTEDUI_MODE_VIDEO_SECURED|TRUSTEDUI_MODE_INPUT_SECURED);
					trustedui_set_mode(TRUSTEDUI_MODE_OFF);
				}
			}

			tui_cover_mode_set(true);
#endif

		} else {
			ts->flip_enable = false;
#ifdef CONFIG_TRUSTONIC_TRUSTED_UI
			tui_cover_mode_set(false);
#endif
		}

		if (!ts->power_status == SEC_TS_STATE_POWER_OFF && ts->reinit_done) {
			if (ts->flip_enable)
				sec_ts_set_cover_type(ts, true);
			else
				sec_ts_set_cover_type(ts, false);
		}

		snprintf(buff, sizeof(buff), "%s", "OK");
		sec->cmd_state = SEC_CMD_STATUS_OK;
	}
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_WAITING;
	sec_cmd_set_cmd_exit(sec);

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
};

static void dead_zone_enable(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	int ret;
	char data = 0;

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1) {
		snprintf(buff, sizeof(buff), "%s", "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
	} else {
		data = sec->cmd_param[0];

		ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_EDGE_DEADZONE, &data, 1);
		if (ret < 0) {
			input_err(true, &ts->client->dev,
					"%s: failed to set deadzone\n", __func__);
			snprintf(buff, sizeof(buff), "%s", "NG");
			sec->cmd_state = SEC_CMD_STATUS_FAIL;
			goto err_set_dead_zone;
		}

		snprintf(buff, sizeof(buff), "%s", "OK");
		sec->cmd_state = SEC_CMD_STATUS_OK;
	}

err_set_dead_zone:
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
};


static void drawing_test_enable(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	int ret;

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1) {
		snprintf(buff, sizeof(buff), "%s", "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
	} else {
		if (ts->use_sponge) {
			if (sec->cmd_param[0])
				ts->lowpower_mode &= ~SEC_TS_MODE_SPONGE_FORCE_KEY;
			else
				ts->lowpower_mode |= SEC_TS_MODE_SPONGE_FORCE_KEY;

			ret = sec_ts_set_custom_library(ts);
			if (ret < 0) {
				snprintf(buff, sizeof(buff), "%s", "NG");
				sec->cmd_state = SEC_CMD_STATUS_FAIL;
			} else {
				snprintf(buff, sizeof(buff), "%s", "OK");
				sec->cmd_state = SEC_CMD_STATUS_OK;
			}

		} else {
			snprintf(buff, sizeof(buff), "%s", "NA");
			sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		}
	}

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
};

static void sec_ts_swap(u8 *a, u8 *b)
{
	u8 temp = *a;

	*a = *b;
	*b = temp;
}

static void rearrange_sft_result(u8 *data, int length)
{
	int i;

	for(i = 0; i < length; i += 4) {
		sec_ts_swap(&data[i], &data[i + 3]);
		sec_ts_swap(&data[i + 1], &data[i + 2]);
	}
}

static int execute_selftest(struct sec_ts_data *ts, bool save_result)
{
	int rc;
	u8 tpara[2] = {0x23, 0x40};
	u8 *rBuff;
	int i;
	int result_size = SEC_TS_SELFTEST_REPORT_SIZE + ts->tx_count * ts->rx_count * 2;

	/* save selftest result in flash */
	if (save_result)
		tpara[0] = 0x23;
	else
		tpara[0] = 0xA3;

	rBuff = kzalloc(result_size, GFP_KERNEL);
	if (!rBuff)
		return -ENOMEM;

	input_info(true, &ts->client->dev, "%s: Self test start!\n", __func__);
	rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SELFTEST, tpara, 2);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: Send selftest cmd failed!\n", __func__);
		goto err_exit;
	}

	sec_ts_delay(350);

	rc = sec_ts_wait_for_ready(ts, SEC_TS_VENDOR_ACK_SELF_TEST_DONE);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: Selftest execution time out!\n", __func__);
		goto err_exit;
	}

	input_info(true, &ts->client->dev, "%s: Self test done!\n", __func__);

	rc = ts->sec_ts_i2c_read(ts, SEC_TS_READ_SELFTEST_RESULT, rBuff, result_size);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: Selftest execution time out!\n", __func__);
		goto err_exit;
	}
	rearrange_sft_result(rBuff, result_size);

	for (i = 0; i < 80; i += 4) {
		if (i % 8 == 0) pr_cont("\n");
		if (i % 4 == 0) pr_cont("%s sec_ts : ", SECLOG);

		if (i / 4 == 0) pr_cont("SIG");
		else if (i / 4 == 1) pr_cont("VER");
		else if (i / 4 == 2) pr_cont("SIZ");
		else if (i / 4 == 3) pr_cont("CRC");
		else if (i / 4 == 4) pr_cont("RES");
		else if (i / 4 == 5) pr_cont("COU");
		else if (i / 4 == 6) pr_cont("PAS");
		else if (i / 4 == 7) pr_cont("FAI");
		else if (i / 4 == 8) pr_cont("CHA");
		else if (i / 4 == 9) pr_cont("AMB");
		else if (i / 4 == 10) pr_cont("RXS");
		else if (i / 4 == 11) pr_cont("TXS");
		else if (i / 4 == 12) pr_cont("RXO");
		else if (i / 4 == 13) pr_cont("TXO");
		else if (i / 4 == 14) pr_cont("RXG");
		else if (i / 4 == 15) pr_cont("TXG");
		else if (i / 4 == 16) pr_cont("RXR");
		else if (i / 4 == 17) pr_cont("TXT");
		else if (i / 4 == 18) pr_cont("RXT");
		else if (i / 4 == 19) pr_cont("TXR");

		pr_cont(" %2X, %2X, %2X, %2X  ", rBuff[i], rBuff[i + 1], rBuff[i + 2], rBuff[i + 3]);

		if (i / 4 == 4) {
			if ((rBuff[i + 3] & 0x30) != 0)// RX, RX open check.
				rc = 0;
			else
				rc = 1;

			ts->ito_test[0] = rBuff[i];
			ts->ito_test[1] = rBuff[i + 1];
			ts->ito_test[2] = rBuff[i + 2];
			ts->ito_test[3] = rBuff[i + 3];
		}
	}

err_exit:
	kfree(rBuff);
	return rc;
}

static void run_trx_short_test(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = {0};
	int rc;
	char para = TO_TOUCH_MODE;

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: Touch is stopped!\n", __func__);
		snprintf(buff, sizeof(buff), "%s", "TSP turned off");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		return;
	}

	disable_irq(ts->client->irq);

	rc = execute_selftest(ts, true);
	if (rc > 0) {
		ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, &para, 1);
		enable_irq(ts->client->irq);
		snprintf(buff, sizeof(buff), "%s", "OK");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_OK;

		input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
		return;
	}

	ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, &para, 1);
	enable_irq(ts->client->irq);

	snprintf(buff, sizeof(buff), "%s", "NG");
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_FAIL;

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
	return;

}

static void run_lowpower_selftest(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };
	char rwbuf[2] = { 0 };
	int ret;

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: Touch is stopped!\n", __func__);
		snprintf(buff, sizeof(buff), "TSP turned off");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		return;
	}

	disable_irq(ts->client->irq);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_STATEMANAGE_ON, rwbuf, 1);
	if (ret < 0)
		goto err_exit;

	rwbuf[0] = 0x5;
	rwbuf[1] = 0x0;
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_CHG_SYSMODE, rwbuf, 2);
	if (ret < 0)
		goto err_exit;

	sec_ts_delay(20);

	rwbuf[0] = 0x1;
	rwbuf[1] = 0x0;
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_START_LOWPOWER_TEST, rwbuf, 1);
	if (ret < 0)
		goto err_exit;

	ret = sec_ts_wait_for_ready(ts,  SEC_TS_VENDOR_ACK_LOWPOWER_SELF_TEST_DONE);
	if (ret < 0)
		goto err_exit;

	rwbuf[0] = 0;
	rwbuf[1] = 0;
	/* success : 1, fail : 0 */
	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_START_LOWPOWER_TEST, rwbuf, 1);
	if (rwbuf[0] != 1) {
		input_err(true, &ts->client->dev,
				"%s: result failed, %d\n", __func__, rwbuf[0]);
		ret = -EIO;
		goto err_exit;
	}

	rwbuf[0] = 0x2;
	rwbuf[1] = 0x2;
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_CHG_SYSMODE, rwbuf, 2);
	if (ret < 0)
		goto err_exit;

	rwbuf[0] = 1;
	rwbuf[1] = 0;
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_STATEMANAGE_ON, rwbuf, 1);
	if (ret < 0)
		goto err_exit;

err_exit:
	enable_irq(ts->client->irq);

	if (ret < 0) {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
	} else {
		snprintf(buff, sizeof(buff), "OK");
		sec->cmd_state = SEC_CMD_STATUS_OK;
	}
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

int sec_tclm_execute_force_calibration(struct i2c_client *client, int cal_mode)
{
	struct sec_ts_data *ts = i2c_get_clientdata(client);
	int rc;
	/* cal_mode is same tclm and sec. if they are different, modify source */
	rc = sec_ts_execute_force_calibration(ts, cal_mode);

	return rc;
}
int sec_ts_execute_force_calibration(struct sec_ts_data *ts, int cal_mode)
{
	int rc = -1;
	u8 cmd;

	input_info(true, &ts->client->dev, "%s: %d\n", __func__, cal_mode);

	if (cal_mode == OFFSET_CAL_SEC)
		cmd = SEC_TS_CMD_FACTORY_PANELCALIBRATION;
	else if (cal_mode == AMBIENT_CAL)
		cmd = SEC_TS_CMD_CALIBRATION_AMBIENT;
#ifdef USE_PRESSURE_SENSOR
	else if (cal_mode == PRESSURE_CAL)
		cmd = SEC_TS_CMD_CALIBRATION_PRESSURE;
#endif
	else
		return rc;

	if (ts->sec_ts_i2c_write(ts, cmd, NULL, 0) < 0) {
		input_err(true, &ts->client->dev, "%s: Write Cal commend failed!\n", __func__);
		return rc;
	}

	sec_ts_delay(1000);

	rc = sec_ts_wait_for_ready(ts, SEC_TS_VENDOR_ACK_OFFSET_CAL_DONE);

	return rc;
}

static void run_force_calibration(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = {0};
	int rc;
	struct sec_ts_test_mode mode;
	char mis_cal_data = 0xF0;

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		snprintf(buff, sizeof(buff), "%s", "TSP turned off");
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		goto out_force_cal_before_irq_ctrl;
	}

	if (ts->touch_count > 0) {
		snprintf(buff, sizeof(buff), "%s", "NG_FINGER_ON");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		goto out_force_cal_before_irq_ctrl;
	}

	disable_irq(ts->client->irq);

	rc = sec_ts_execute_force_calibration(ts, OFFSET_CAL_SEC);
	if (rc < 0) {
		snprintf(buff, sizeof(buff), "%s", "FAIL");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		goto out_force_cal;
	}

#if 0//def USE_PRESSURE_SENSOR Kelly don't use press sensor
	rc = sec_ts_execute_force_calibration(ts, PRESSURE_CAL);
	if (rc < 0)
		input_err(true, &ts->client->dev, "%s: fail to write PRESSURE CAL!\n", __func__);
#endif

	if (ts->plat_data->mis_cal_check) {
		buff[0] = 0;
		rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_STATEMANAGE_ON, buff, 1);
		if (rc < 0) {
			input_err(true, &ts->client->dev,
					"%s: mis_cal_check error[1] ret: %d\n", __func__, rc);
		}

		buff[0] = 0x2;
		buff[1] = 0x2;
		rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_CHG_SYSMODE, buff, 2);
		if (rc < 0) {
			input_err(true, &ts->client->dev,
					"%s: mis_cal_check error[2] ret: %d\n", __func__, rc);
		}

		input_err(true, &ts->client->dev, "%s: try mis Cal. check\n", __func__);
		rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_MIS_CAL_CHECK, NULL, 0);
		if (rc < 0) {
			input_err(true, &ts->client->dev,
					"%s: mis_cal_check error[3] ret: %d\n", __func__, rc);
		}
		sec_ts_delay(200);

		rc = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_MIS_CAL_READ, &mis_cal_data, 1);
		if (rc < 0) {
			input_err(true, &ts->client->dev, "%s: i2c fail!, %d\n", __func__, rc);
			mis_cal_data = 0xF3;
		} else {
			input_info(true, &ts->client->dev, "%s: miss cal data : %d\n", __func__, mis_cal_data);
		}

		buff[0] = 1;
		rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_STATEMANAGE_ON, buff, 1);
		if (rc < 0) {
			input_err(true, &ts->client->dev,
					"%s: mis_cal_check error[4] ret: %d\n", __func__, rc);
		}

		if (mis_cal_data) {
			memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
			mode.type = TYPE_AMBIENT_DATA;
			mode.allnode = TEST_MODE_ALL_NODE;

			sec_ts_read_raw_data(ts, NULL, &mode);
			snprintf(buff, sizeof(buff), "%s", "MIS CAL");
			sec->cmd_state = SEC_CMD_STATUS_FAIL;
			goto out_force_cal;
		}
	}

#ifdef TCLM_CONCEPT
#ifdef CONFIG_DUAL_TSP
	/* exit TCLM NV write about 0=main TSP */
	if (!(gpio_get_value(ts->plat_data->tsp_sel) == 0))
#endif
	{
		/* devide tclm case */
		sec_tclm_case(ts->tdata, sec->cmd_param[0]);

		input_info(true, &ts->client->dev, "%s: param, %d, %c, %d\n", __func__,
			sec->cmd_param[0], sec->cmd_param[0], ts->tdata->root_of_calibration);

		sec_execute_tclm_package(ts->tdata, 1);
		sec_tclm_root_of_cal(ts->tdata, CALPOSITION_NONE);
	}
#endif

	ts->cal_status = sec_ts_read_calibration_report(ts);
	snprintf(buff, sizeof(buff), "%s", "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;

out_force_cal:
	enable_irq(ts->client->irq);

out_force_cal_before_irq_ctrl:
	/* not to enter external factory mode without setting everytime */
	ts->tdata->external_factory = false;

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);

}

static void get_force_calibration(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = {0};
	int rc;

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: Touch is stopped!\n", __func__);
		snprintf(buff, sizeof(buff), "%s", "TSP turned off");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		return;
	}

	rc = sec_ts_read_calibration_report(ts);
	if (rc < 0) {
		snprintf(buff, sizeof(buff), "%s", "FAIL");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
	} else if (rc == SEC_TS_STATUS_CALIBRATION_SEC) {
		snprintf(buff, sizeof(buff), "%s", "OK");
		sec->cmd_state = SEC_CMD_STATUS_OK;
	} else {
		snprintf(buff, sizeof(buff), "%s", "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
	}

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);

}

#ifdef USE_PRESSURE_SENSOR
static void run_force_pressure_calibration(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = {0};
	int rc;
	char data[3] = { 0 };

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: Touch is stopped!\n", __func__);
		snprintf(buff, sizeof(buff), "%s", "TSP turned off");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		return;
	}

	if (ts->touch_count > 0) {
		snprintf(buff, sizeof(buff), "%s", "NG_FINGER_ON");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		goto out_force_pressure_cal;
	}

	disable_irq(ts->client->irq);

	rc = sec_ts_execute_force_calibration(ts, PRESSURE_CAL);
	if (rc < 0) {
		snprintf(buff, sizeof(buff), "%s", "FAIL");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
	} else {
		snprintf(buff, sizeof(buff), "%s", "OK");
		sec->cmd_state = SEC_CMD_STATUS_OK;
	}

	ts->pressure_cal_base = get_tsp_nvm_data(ts, SEC_TS_NVM_OFFSET_PRESSURE_BASE_CAL_COUNT);
	if (ts->pressure_cal_base == 0xFF)
		ts->pressure_cal_base = 0;
	if (ts->pressure_cal_base > 0xFD)
		ts->pressure_cal_base = 0xFD;

	/* Use TSP NV area : in this model, use only one byte
	 * data[0] : offset from user NVM storage
	 * data[1] : length of stroed data - 1 (ex. using 1byte, value is  1 - 1 = 0)
	 * data[2] : write data
	 */
	data[0] = SEC_TS_NVM_OFFSET_PRESSURE_BASE_CAL_COUNT;
	data[1] = 0;
	data[2] = ts->pressure_cal_base + 1;

	rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_NVM, data, 3);
	if (rc < 0)
		input_err(true, &ts->client->dev,
				"%s: nvm write failed. ret: %d\n", __func__, rc);

	ts->pressure_cal_base = get_tsp_nvm_data(ts, SEC_TS_NVM_OFFSET_PRESSURE_BASE_CAL_COUNT);

	input_info(true, &ts->client->dev, "%s: count:%d\n", __func__, ts->pressure_cal_base);

	enable_irq(ts->client->irq);

out_force_pressure_cal:
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);

}

static void set_pressure_test_mode(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = {0};
	int ret;
	unsigned char data = TYPE_INVALID_DATA;
	unsigned char enable = 0;

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: Touch is stopped!\n", __func__);
		snprintf(buff, sizeof(buff), "%s", "TSP turned off");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		return;
	}

	if (sec->cmd_param[0] == 1) {
		enable = 0x1;
		ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_TEMPERATURE_COMP_MODE, &enable, 1);
		if (ret < 0) {
			snprintf(buff, sizeof(buff), "%s", "NG");
			sec->cmd_state = SEC_CMD_STATUS_FAIL;
			goto out_test_mode;
		}

		ret = sec_ts_fix_tmode(ts, TOUCH_SYSTEM_MODE_TOUCH, TOUCH_MODE_STATE_TOUCH);
		if (ret < 0) {
			snprintf(buff, sizeof(buff), "%s", "NG");
			sec->cmd_state = SEC_CMD_STATUS_FAIL;
			goto out_test_mode;
		}

	} else {
		ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SELECT_PRESSURE_TYPE, &data, 1);
		if (ret < 0) {
			snprintf(buff, sizeof(buff), "%s", "NG");
			sec->cmd_state = SEC_CMD_STATUS_FAIL;
			goto out_test_mode;
		}

		ret = sec_ts_release_tmode(ts);
		if (ret < 0) {
			snprintf(buff, sizeof(buff), "%s", "NG");
			sec->cmd_state = SEC_CMD_STATUS_FAIL;
			goto out_test_mode;
		}

		enable = 0x0;
		ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_TEMPERATURE_COMP_MODE, &enable, 1);
		if (ret < 0) {
			snprintf(buff, sizeof(buff), "%s", "NG");
			sec->cmd_state = SEC_CMD_STATUS_FAIL;
			goto out_test_mode;
		}
	}

	snprintf(buff, sizeof(buff), "%s", "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;

out_test_mode:
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

int read_pressure_data(struct sec_ts_data *ts, u8 type, short *value)
{
	unsigned char data[6] = { 0 };
	short pressure[3] = { 0 };
	int ret, i;

	if (ts->power_status == SEC_TS_STATE_POWER_OFF)
		return -ENODEV;

	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_SELECT_PRESSURE_TYPE, data, 1);
	if (ret < 0)
		return -EIO;

	if (data[0] != type) {
		data[1] = type;

		ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SELECT_PRESSURE_TYPE, &data[1], 1);
		if (ret < 0)
			return -EIO;

		sec_ts_delay(30);
	}

	memset(data, 0x00, 6);

	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_READ_PRESSURE_DATA, data, 6);
	if (ret < 0)
		return -EIO;

	pressure[0] = (data[0] << 8 | data[1]);
	pressure[1] = (data[2] << 8 | data[3]);
	pressure[2] = (data[4] << 8 | data[5]);

	input_raw_info(true, &ts->client->dev, "%s: Type:%d, Left: %d, Center: %d, Rignt: %d\n",
			__func__, type, pressure[2], pressure[1], pressure[0]);

	memcpy(value, pressure, 3 * 2);

	for (i = 0; i < PRESSURE_CHANNEL_NUM; i++) {
		ts->pressure_data[type][i] = pressure[i];
	}

	return ret;
}

static void run_pressure_filtered_strength_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = {0};
	short pressure[3] = { 0 };
	int ret;

	sec_cmd_set_default_result(sec);

	ret = read_pressure_data(ts, TYPE_SIGNAL_DATA, pressure);
	if (ret < 0) {
		snprintf(buff, sizeof(buff), "%s", "WRITE FAILED");
		goto error_read_str;
	}

	snprintf(buff, sizeof(buff), "%d,%d,%d", pressure[2], pressure[1], pressure[0]);

	sec->cmd_state = SEC_CMD_STATUS_OK;

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);

	return;

error_read_str:
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
}

static void run_pressure_strength_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = {0};
	short pressure[3] = { 0 };
	int ret;

	sec_cmd_set_default_result(sec);

	ret = read_pressure_data(ts, TYPE_REMV_AMB_DATA, pressure);
	if (ret < 0) {
		snprintf(buff, sizeof(buff), "%s", "WRITE FAILED");
		goto error_read_str;
	}

	snprintf(buff, sizeof(buff), "%d,%d,%d", pressure[2], pressure[1], pressure[0]);

	sec->cmd_state = SEC_CMD_STATUS_OK;

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);

	return;

error_read_str:
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
}

static void run_pressure_rawdata_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = {0};
	short pressure[3] = { 0 };
	int ret;

	sec_cmd_set_default_result(sec);

	ret = read_pressure_data(ts, TYPE_RAW_DATA, pressure);
	if (ret < 0) {
		snprintf(buff, sizeof(buff), "%s", "WRITE FAILED");
		goto error_read_rawdata;
	}

	snprintf(buff, sizeof(buff), "%d,%d,%d", pressure[2], pressure[1], pressure[0]);

	sec->cmd_state = SEC_CMD_STATUS_OK;

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);

	return;

error_read_rawdata:
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
}

static void run_pressure_offset_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = {0};
	short pressure[3] = { 0 };
	int ret;

	sec_cmd_set_default_result(sec);

	ret = read_pressure_data(ts, TYPE_OFFSET_DATA_SEC, pressure);
	if (ret < 0) {
		snprintf(buff, sizeof(buff), "%s", "WRITE FAILED");
		goto error_read_str;
	}

	snprintf(buff, sizeof(buff), "%d,%d,%d", pressure[2], pressure[1], pressure[0]);

	sec->cmd_state = SEC_CMD_STATUS_OK;

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);

	return;

error_read_str:
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
}

static void set_pressure_strength(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = {0};
	u8 data[8] = { 0 };
	u8 cal_data[18] = { 0 };
	int index;
	int ret;
	short pressure[3] = { 0 };

	sec_cmd_set_default_result(sec);

	if ((sec->cmd_param[0] < 1) || (sec->cmd_param[0] > 4))
		goto err_cmd_param_str;

	index = sec->cmd_param[0] - 1;

	/* RIGHT */
	cal_data[0] = (sec->cmd_param[3] >> 8);
	cal_data[1] = (sec->cmd_param[3] & 0xFF);
	/* CENTER */
	cal_data[8] = (sec->cmd_param[2] >> 8);
	cal_data[9] = (sec->cmd_param[2] & 0xFF);
	/* LEFT */
	cal_data[16] = (sec->cmd_param[1] >> 8);
	cal_data[17] = (sec->cmd_param[1] & 0xFF);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_GET_PRESSURE, cal_data, 18);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: cmd write failed. ret: %d\n", __func__, ret);
		goto err_comm_str;
	}

	sec_ts_delay(30);

	memset(cal_data, 0x00, 18);
	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_SET_GET_PRESSURE, cal_data, 18);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: cmd write failed. ret: %d\n", __func__, ret);
		goto err_comm_str;
	}

	pressure[0] = ((cal_data[16] << 8) | cal_data[17]);
	pressure[1] = ((cal_data[8] << 8) | cal_data[9]);
	pressure[2] = ((cal_data[0] << 8) | cal_data[1]);

	input_info(true, &ts->client->dev, "%s: [%d] : %d, %d, %d\n", __func__, pressure[0], pressure[1], pressure[2]);

	/* Use TSP NV area : in this model, use only one byte
	 * buff[0] : offset from user NVM storage
	 * buff[1] : [n] length of stroed data - 1 (ex. using 1byte, value is  1 - 1 = 0)
	 * buff[2] ... [n] : write data ...
	 */
	data[0] = SEC_TS_NVM_OFFSET_PRESSURE_STRENGTH + (index * SEC_TS_NVM_SIZE_PRESSURE_BLOCK);
	data[1] = SEC_TS_NVM_SIZE_PRESSURE_BLOCK - 1;
	/* RIGHT */
	data[2] = (sec->cmd_param[3] >> 8);
	data[3] = (sec->cmd_param[3] & 0xFF);
	/* CENTER */
	data[4] = (sec->cmd_param[2] >> 8);
	data[5] = (sec->cmd_param[2] & 0xFF);
	/*LEFT */
	data[6] = (sec->cmd_param[1] >> 8);
	data[7] = (sec->cmd_param[1] & 0xFF);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_NVM, data, 8);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: nvm write failed. ret: %d\n", __func__, ret);
		goto err_comm_str;
	}

	sec_ts_delay(20);

	input_info(true, &ts->client->dev, "%s: [%d] : %d, %d, %d\n",
			__func__, index, (data[6] << 8) + data[7], (data[4] << 8) + data[5], (data[0] << 8) + data[1]);

	memset(data, 0x00, 8);

	data[0] = SEC_TS_NVM_OFFSET_PRESSURE_INDEX;
	data[1] = 0;
	data[2] = (u8)(index & 0xFF);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_NVM, data, 3);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: nvm write failed. ret: %d\n", __func__, ret);
		goto err_comm_str;
	}

	sec_ts_delay(20);

	snprintf(buff, sizeof(buff), "%s", "OK");

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;

	ts->pressure_cal_delta = get_tsp_nvm_data(ts, SEC_TS_NVM_OFFSET_PRESSURE_DELTA_CAL_COUNT);
	if (ts->pressure_cal_delta == 0xFF)
		ts->pressure_cal_delta = 0;

	if (ts->pressure_cal_delta > 0xFD)
		ts->pressure_cal_delta = 0xFD;

	/* Use TSP NV area : in this model, use only one byte
	 * data[0] : offset from user NVM storage
	 * data[1] : length of stroed data - 1 (ex. using 1byte, value is  1 - 1 = 0)
	 * data[2] : write data
	 */
	data[0] = SEC_TS_NVM_OFFSET_PRESSURE_DELTA_CAL_COUNT;
	data[1] = 0;
	data[2] = ts->pressure_cal_delta + 1;

	ret= ts->sec_ts_i2c_write(ts, SEC_TS_CMD_NVM, data, 3);
	if (ret < 0)
		input_err(true, &ts->client->dev,
				"%s: nvm write failed. ret: %d\n", __func__, ret);

	ts->pressure_cal_delta = get_tsp_nvm_data(ts, SEC_TS_NVM_OFFSET_PRESSURE_DELTA_CAL_COUNT);

	input_info(true, &ts->client->dev, "%s: count:%d\n", __func__, ts->pressure_cal_delta);

	return;

err_cmd_param_str:
	input_info(true, &ts->client->dev, "%s: parameter error: %u\n",
			__func__, sec->cmd_param[0]);
err_comm_str:
	snprintf(buff, sizeof(buff), "NG");
	sec_cmd_set_cmd_result(sec, buff, strlen(buff));

	sec->cmd_state = SEC_CMD_STATUS_FAIL;
}

static void set_pressure_rawdata(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = {0};
	u8 data[8] = { 0 };
	int index;
	int ret;

	sec_cmd_set_default_result(sec);

	if ((sec->cmd_param[0] < 1) || (sec->cmd_param[0] > 4))
		goto err_cmd_param_raw;

	index = sec->cmd_param[0] - 1;

	/* Use TSP NV area : in this model, use only one byte
	 * buff[0] : offset from user NVM storage
	 * buff[1] : [n] length of stroed data - 1 (ex. using 1byte, value is  1 - 1 = 0)
	 * buff[2] ... [n] : write data ...
	 */
	data[0] = SEC_TS_NVM_OFFSET_PRESSURE_RAWDATA + (index * SEC_TS_NVM_SIZE_PRESSURE_BLOCK);
	data[1] = SEC_TS_NVM_SIZE_PRESSURE_BLOCK - 1;
	data[2] = (sec->cmd_param[3] >> 8);
	data[3] = (sec->cmd_param[3] & 0xFF);
	data[4] = (sec->cmd_param[2] >> 8);
	data[5] = (sec->cmd_param[2] & 0xFF);
	data[6] = (sec->cmd_param[1] >> 8);
	data[7] = (sec->cmd_param[1] & 0xFF);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_NVM, data, 8);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: nvm write failed. ret: %d\n", __func__, ret);
		goto err_comm_raw;
	}
	sec_ts_delay(20);

	memset(data, 0x00, 8);

	ret = get_tsp_nvm_data_by_size(ts, SEC_TS_NVM_OFFSET_PRESSURE_RAWDATA + (index * SEC_TS_NVM_SIZE_PRESSURE_BLOCK), SEC_TS_NVM_SIZE_PRESSURE_BLOCK, data);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: nvm read failed. ret: %d\n", __func__, ret);
		goto err_comm_raw;
	}

	snprintf(buff, sizeof(buff), "%s", "OK");

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;

	input_info(true, &ts->client->dev, "%s: [%d] : %d, %d, %d\n",
			__func__, index, (data[4] << 8) + data[5], (data[2] << 8) + data[3], (data[1] << 8) + data[0]);

	return;

err_cmd_param_raw:
	input_info(true, &ts->client->dev, "%s: parameter error: %u\n",
			__func__, sec->cmd_param[0]);
err_comm_raw:

	snprintf(buff, sizeof(buff), "NG");
	sec_cmd_set_cmd_result(sec, buff, strlen(buff));

	sec->cmd_state = SEC_CMD_STATUS_FAIL;
}

static void set_pressure_data_index(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = {0};
	u8 data[8] = { 0 };
	u8 cal_data[18] = { 0 };
	int index;
	int ret;

	sec_cmd_set_default_result(sec);

	if ((sec->cmd_param[0] < 0) || (sec->cmd_param[0] > 4))
		goto err_set_cmd_param_index;

	if (sec->cmd_param[0] == 0) {
		input_info(true, &ts->client->dev, "%s: clear calibration result\n", __func__);
		/* clear pressure calibrated data */
		ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_GET_PRESSURE, cal_data, 18);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: cmd write failed. ret: %d\n", __func__, ret);
			goto err_set_comm_index;
		}

		sec_ts_delay(30);

		goto clear_index;
	}

	index = sec->cmd_param[0] - 1;

	ret = get_tsp_nvm_data_by_size(ts, SEC_TS_NVM_OFFSET_PRESSURE_STRENGTH, 24, data);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: nvm read failed. ret: %d\n", __func__, ret);
		goto err_set_comm_index;
	}

	cal_data[16] = data[6 * index + 4];
	cal_data[17] = data[6 * index + 5];	/* LEFT */

	cal_data[8] = data[6 * index + 2];
	cal_data[9] = data[6 * index + 3];	/* CENTER */

	cal_data[0] = data[6 * index + 0];
	cal_data[1] = data[6 * index + 1];	/* RIGHT */

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_GET_PRESSURE, cal_data, 18);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: cmd write failed. ret: %d\n", __func__, ret);
		goto err_set_comm_index;
	}

	sec_ts_delay(30);

	/* Use TSP NV area : in this model, use only one byte
	 * buff[0] : offset from user NVM storage
	 * buff[1] : length of stroed data - 1 (ex. using 1byte, value is  1 - 1 = 0)
	 * buff[2] : write data
	 */
	memset(data, 0x00, 8);
	data[0] = SEC_TS_NVM_OFFSET_PRESSURE_INDEX;
	data[1] = 0;
	data[2] = (u8)(index & 0xFF);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_NVM, data, 3);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: nvm write failed. ret: %d\n", __func__, ret);
		goto err_set_comm_index;
	}

	sec_ts_delay(20);

clear_index:
	snprintf(buff, sizeof(buff), "%s", "OK");

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;

	return;

err_set_cmd_param_index:
	input_info(true, &ts->client->dev, "%s: parameter error: %u\n",
			__func__, sec->cmd_param[0]);
err_set_comm_index:

	snprintf(buff, sizeof(buff), "NG");
	sec_cmd_set_cmd_result(sec, buff, strlen(buff));

	sec->cmd_state = SEC_CMD_STATUS_FAIL;
	return;

}
static void get_pressure_strength(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = {0};
	int index;
	u8 data[24] = { 0 };
	short pressure[3] = { 0 };
	int ret;

	sec_cmd_set_default_result(sec);

	if ((sec->cmd_param[0] < 1) || (sec->cmd_param[0] > 4))
		goto err_get_cmd_param_str;

	index = sec->cmd_param[0] - 1;

	ret = get_tsp_nvm_data_by_size(ts, SEC_TS_NVM_OFFSET_PRESSURE_STRENGTH, 24, data);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: nvm read failed. ret: %d\n", __func__, ret);
		goto err_get_comm_str;
	}

	pressure[0] = ((data[6 * index + 4] << 8) + data[6 * index + 5]);
	pressure[1] = ((data[6 * index + 2] << 8) + data[6 * index + 3]);
	pressure[2] = ((data[6 * index + 0] << 8) + data[6 * index + 1]);

	snprintf(buff, sizeof(buff), "%d,%d,%d", pressure[0], pressure[1], pressure[2]);

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;

	input_info(true, &ts->client->dev, "%s: [%d] : %d, %d, %d\n",
			__func__, index, pressure[0], pressure[1], pressure[2]);

	return;

err_get_comm_str:
	input_info(true, &ts->client->dev, "%s: parameter error: %u\n",
			__func__, sec->cmd_param[0]);
err_get_cmd_param_str:

	snprintf(buff, sizeof(buff), "NG");
	sec_cmd_set_cmd_result(sec, buff, strlen(buff));

	sec->cmd_state = SEC_CMD_STATUS_FAIL;
}

static void get_pressure_rawdata(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = {0};
	int index;
	u8 data[24] = { 0 };
	short pressure[3] = { 0 };
	int ret;

	sec_cmd_set_default_result(sec);

	if ((sec->cmd_param[0] < 1) || (sec->cmd_param[0] > 4))
		goto err_get_cmd_param_raw;

	index = sec->cmd_param[0] - 1;

	ret = get_tsp_nvm_data_by_size(ts, SEC_TS_NVM_OFFSET_PRESSURE_RAWDATA, 24, data);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: nvm read failed. ret: %d\n", __func__, ret);
		goto err_get_comm_raw;
	}

	pressure[0] = ((data[6 * index + 4] << 8) + data[6 * index + 5]);
	pressure[1] = ((data[6 * index + 2] << 8) + data[6 * index + 3]);
	pressure[2] = ((data[6 * index + 0] << 8) + data[6 * index + 1]);

	snprintf(buff, sizeof(buff), "%d,%d,%d", pressure[0], pressure[1], pressure[2]);

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;

	input_info(true, &ts->client->dev, "%s: [%d] : %d, %d, %d\n",
			__func__, index, pressure[0], pressure[1], pressure[2]);

	return;
err_get_cmd_param_raw:
	input_info(true, &ts->client->dev, "%s: parameter error: %u\n",
			__func__, sec->cmd_param[0]);
err_get_comm_raw:

	snprintf(buff, sizeof(buff), "NG");
	sec_cmd_set_cmd_result(sec, buff, strlen(buff));

	sec->cmd_state = SEC_CMD_STATUS_FAIL;
}

static void get_pressure_data_index(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = {0};
	int index = 0;

	sec_cmd_set_default_result(sec);

	index = get_tsp_nvm_data(ts, SEC_TS_NVM_OFFSET_PRESSURE_INDEX);
	if (index < 0) {
		goto err_get_index;
	} else {
		if (index == 0xFF)
			snprintf(buff, sizeof(buff), "%d", 0);
		else
			snprintf(buff, sizeof(buff), "%d", index + 1);
	}

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;

	input_info(true, &ts->client->dev, "%s: %d\n",
			__func__, index);

	return;

err_get_index:

	snprintf(buff, sizeof(buff), "NG");
	sec_cmd_set_cmd_result(sec, buff, strlen(buff));

	sec->cmd_state = SEC_CMD_STATUS_FAIL;
	return;

}
static void set_pressure_strength_clear(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = {0};
	u8 *data;
	u8 cal_data[18] = { 0 };
	int ret;

	sec_cmd_set_default_result(sec);

	/* clear pressure calibrated data */
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_GET_PRESSURE, cal_data, 18);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: cmd write failed. ret: %d\n", __func__, ret);
		goto err_comm_str;
	}

	sec_ts_delay(30);

	/* Use TSP NV area : in this model, use only one byte
	 * buff[0] : offset from user NVM storage
	 * buff[1] : length of stroed data - 1 (ex. using 1byte, value is  1 - 1 = 0)
	 * buff[2] : write data
	 */

	/* strength 6 * 4,  rawdata 6 * 4, buff[0], buff[1] */
	data = kzalloc(50, GFP_KERNEL);
	if (!data) {
		input_err(true, &ts->client->dev, "%s failed to allocate memory. ret: %d\n", __func__, ret);
		goto err_comm_str;
	}

	data[0] = SEC_TS_NVM_OFFSET_PRESSURE_INDEX;
	data[1] = (SEC_TS_NVM_SIZE_PRESSURE_BLOCK * 8) - 1;

	/* remove calicated strength, rawdata in NVM */
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_NVM, data, 50);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: nvm write failed. ret: %d\n", __func__, ret);
		goto err_mem_str;
	}

	sec_ts_delay(20);

	snprintf(buff, sizeof(buff), "%s", "OK");

	kfree(data);

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;

	return;

err_mem_str:
	kfree(data);
err_comm_str:
	snprintf(buff, sizeof(buff), "NG");
	sec_cmd_set_cmd_result(sec, buff, strlen(buff));

	sec->cmd_state = SEC_CMD_STATUS_FAIL;
}

static void get_pressure_threshold(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	char buff[SEC_CMD_STR_LEN] = {0};

	sec_cmd_set_default_result(sec);

	snprintf(buff, sizeof(buff), "300");

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
}

/* low level is more sensitivity, except level-0(value 0) */
static void set_pressure_user_level(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = {0};
	int ret;
	char addr[3] = { 0 };
	char data[2] = { 0 };
	int pressure_thd = 0;

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: Touch is stopped!\n", __func__);
		snprintf(buff, sizeof(buff), "%s", "TSP turned off");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		return;
	}

	if ((sec->cmd_param[0] < 1) || (sec->cmd_param[0] > 5))
		goto out_set_user_level;

	/*
	 * byte[0]: m_sponge_ifpacket_addr[7:0]
	 * byte[1]: m_sponge_ifpacket_addr[15:8]
	 * byte[n] : user data (max 32 bytes)
	 */
	addr[0] = SEC_TS_CMD_SPONGE_OFFSET_PRESSURE_LEVEL;
	addr[1] = 0x00;
	addr[2] = sec->cmd_param[0];

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SPONGE_WRITE_PARAM, addr, 3);
	if (ret < 0)
		goto out_set_user_level;

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SPONGE_NOTIFY_PACKET, NULL, 0);
	if (ret < 0)
		goto out_set_user_level;

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SPONGE_READ_PARAM, addr, 2);
	if (ret < 0)
		goto out_set_user_level;

	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_SPONGE_READ_PARAM, data, 1);
	if (ret < 0)
		goto out_set_user_level;

	input_info(true, &ts->client->dev, "%s: set user level: %d\n", __func__, data[0]);

	ts->pressure_user_level = data[0];

	addr[0] = SEC_TS_CMD_SPONGE_OFFSET_PRESSURE_THD_HIGH;
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SPONGE_READ_PARAM, addr, 2);
	if (ret < 0)
		goto out_set_user_level;

	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_SPONGE_READ_PARAM, data, 2);
	if (ret < 0)
		goto out_set_user_level;

	pressure_thd = data[0] | data[1] << 8;
	input_info(true, &ts->client->dev, "%s: HIGH THD: %d\n", __func__, pressure_thd);

	addr[0] = SEC_TS_CMD_SPONGE_OFFSET_PRESSURE_THD_LOW;

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SPONGE_READ_PARAM, addr, 2);
	if (ret < 0)
		goto out_set_user_level;

	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_SPONGE_READ_PARAM, data, 2);
	if (ret < 0)
		goto out_set_user_level;

	pressure_thd = data[0] | data[1] << 8;
	input_info(true, &ts->client->dev, "%s: LOW THD: %d\n", __func__, pressure_thd);

	snprintf(buff, sizeof(buff), "%s", "OK");

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	return;

out_set_user_level:
	snprintf(buff, sizeof(buff), "%s", "NG");
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_FAIL;
}

static void get_pressure_user_level(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = {0};
	char addr[3] = { 0 };
	char data[2] = { 0 };
	int ret;

	sec_cmd_set_default_result(sec);

	snprintf(buff, sizeof(buff), "%d", ts->pressure_user_level);

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));

	addr[0] = SEC_TS_CMD_SPONGE_OFFSET_PRESSURE_LEVEL;
	addr[1] = 0x00;

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SPONGE_READ_PARAM, addr, 2);
	if (ret < 0)
		goto out_get_user_level;

	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_SPONGE_READ_PARAM, data, 1);
	if (ret < 0)
		goto out_get_user_level;

	input_err(true, &ts->client->dev, "%s: set user level: %d\n", __func__, data[0]);
	ts->pressure_user_level = data[0];

	snprintf(buff, sizeof(buff), "%s", "OK");
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	return;

out_get_user_level:
	snprintf(buff, sizeof(buff), "%s", "NG");
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_FAIL;

}
#endif

static void factory_cmd_result_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };

	sec->item_count = 0;
	memset(sec->cmd_result_all, 0x00, SEC_CMD_RESULT_STR_LEN);

	if (ts->tsp_dump_lock == 1) {
		input_err(true, &ts->client->dev, "%s: already checking now\n", __func__);
		sec->cmd_all_factory_state = SEC_CMD_STATUS_FAIL;
		goto out;
	}
	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: IC is power off\n", __func__);
		sec->cmd_all_factory_state = SEC_CMD_STATUS_FAIL;
		goto out;
	}

	sec->cmd_all_factory_state = SEC_CMD_STATUS_RUNNING;

	snprintf(buff, sizeof(buff), "%d", ts->plat_data->item_version);
	sec_cmd_set_cmd_result_all(sec, buff, sizeof(buff), "ITEM_VERSION");

	get_chip_vendor(sec);
	get_chip_name(sec);
	get_fw_ver_bin(sec);
	get_fw_ver_ic(sec);

	run_rawcap_read(sec);
	get_gap_data(sec);
	run_reference_read(sec);

	run_self_rawcap_read(sec);
	get_self_channel_data(sec, TYPE_OFFSET_DATA_SDC);
	//run_self_reference_read(sec);

	run_raw_self_p2p_diff_read_all(sec);
	get_p2p_read_one_cmd(sec);

	get_wet_mode(sec);
#ifdef CONFIG_DUAL_TSP
	/* exit TCLM NV write about 0=main TSP */
	if (gpio_get_value(ts->plat_data->tsp_sel) == 0)
		get_mis_cal_info_main(sec);
	else
#endif
	get_mis_cal_info(sec);
	//get_evt_info(sec);

	sec->cmd_all_factory_state = SEC_CMD_STATUS_OK;

out:
	input_info(true, &ts->client->dev, "%s: %d%s\n", __func__, sec->item_count, sec->cmd_result_all);
}

static void set_lowpower_mode(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	char buff[SEC_CMD_STR_LEN] = { 0 };

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1) {
		snprintf(buff, sizeof(buff), "%s", "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
	} else {
		snprintf(buff, sizeof(buff), "%s", "OK");
		sec->cmd_state = SEC_CMD_STATUS_OK;
	}

#if 0
	/* set lowpower mode by spay, edge_swipe function. */
	ts->lowpower_mode = sec->cmd_param[0];
#endif
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));

	sec_cmd_set_cmd_exit(sec);

	return;

}

static void set_wirelesscharger_mode(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	int ret;
	bool mode;
	u8 w_data[1] = {0x00};

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 3)
		goto OUT;

	if (sec->cmd_param[0] == 0) {
		ts->charger_mode |= SEC_TS_BIT_CHARGER_MODE_NO;
		mode = false;
	} else {
		ts->charger_mode &= (~SEC_TS_BIT_CHARGER_MODE_NO);
		mode = true;
	}

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: fail to enable w-charger status, POWER_STATUS=OFF\n", __func__);
		goto NG;
	}

	if (sec->cmd_param[0] == 1)
		ts->charger_mode = ts->charger_mode | SEC_TS_BIT_CHARGER_MODE_WIRELESS_CHARGER;
	else if (sec->cmd_param[0] == 3)
		ts->charger_mode = ts->charger_mode | SEC_TS_BIT_CHARGER_MODE_WIRELESS_BATTERY_PACK;
	else if (mode == false)
		ts->charger_mode = ts->charger_mode & (~SEC_TS_BIT_CHARGER_MODE_WIRELESS_CHARGER) & (~SEC_TS_BIT_CHARGER_MODE_WIRELESS_BATTERY_PACK);

	w_data[0] = ts->charger_mode;
	ret = ts->sec_ts_i2c_write(ts, SET_TS_CMD_SET_CHARGER_MODE, w_data, 1);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: Failed to send command 74\n", __func__);
		goto NG;
	}

	input_err(true, &ts->client->dev, "%s: %s, status =%x\n",
			__func__, (mode) ? "wireless enable" : "wireless disable", ts->charger_mode);

	snprintf(buff, sizeof(buff), "%s", "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
	return;

NG:
	input_err(true, &ts->client->dev, "%s: %s, status =%x\n",
			__func__, (mode) ? "wireless enable" : "wireless disable", ts->charger_mode);

OUT:
	snprintf(buff, sizeof(buff), "%s", "NG");
	sec->cmd_state = SEC_CMD_STATUS_FAIL;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
}

static void spay_enable(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1)
		goto NG;

	if (sec->cmd_param[0]) {
		if (ts->use_sponge)
			ts->lowpower_mode |= SEC_TS_MODE_SPONGE_SPAY;
	} else {
		if (ts->use_sponge)
			ts->lowpower_mode &= ~SEC_TS_MODE_SPONGE_SPAY;
	}

	input_info(true, &ts->client->dev, "%s: %02X\n", __func__, ts->lowpower_mode);

	if (ts->use_sponge)
		sec_ts_set_custom_library(ts);

	snprintf(buff, sizeof(buff), "%s", "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
	return;

NG:
	snprintf(buff, sizeof(buff), "%s", "NG");
	sec->cmd_state = SEC_CMD_STATUS_FAIL;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
}

static void set_aod_rect(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	u8 data[10] = {0x02, 0};
	int ret, i;

	sec_cmd_set_default_result(sec);

	input_info(true, &ts->client->dev, "%s: w:%d, h:%d, x:%d, y:%d\n",
			__func__, sec->cmd_param[0], sec->cmd_param[1],
			sec->cmd_param[2], sec->cmd_param[3]);

	for (i = 0; i < 4; i++) {
		data[i * 2 + 2] = sec->cmd_param[i] & 0xFF;
		data[i * 2 + 3] = (sec->cmd_param[i] >> 8) & 0xFF;
		ts->rect_data[i] = sec->cmd_param[i];
	}

	if (ts->use_sponge) {
		disable_irq(ts->client->irq);
		ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SPONGE_WRITE_PARAM, &data[0], 10);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: Failed to write offset\n", __func__);
			goto NG;
		}

		ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SPONGE_NOTIFY_PACKET, NULL, 0);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: Failed to send notify\n", __func__);
			goto NG;
		}
		enable_irq(ts->client->irq);
	}

	snprintf(buff, sizeof(buff), "%s", "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
	return;
NG:
	enable_irq(ts->client->irq);
	snprintf(buff, sizeof(buff), "%s", "NG");
	sec->cmd_state = SEC_CMD_STATUS_FAIL;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
}


static void get_aod_rect(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	u8 data[8] = {0x02, 0};
	u16 rect_data[4] = {0, };
	int ret, i;

	sec_cmd_set_default_result(sec);

	if (ts->use_sponge) {
		disable_irq(ts->client->irq);
		ret = ts->sec_ts_read_sponge(ts, data, 8);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: Failed to read rect\n", __func__);
			goto NG;
		}
		enable_irq(ts->client->irq);
	}

	for (i = 0; i < 4; i++)
		rect_data[i] = (data[i * 2 + 1] & 0xFF) << 8 | (data[i * 2] & 0xFF);

	input_info(true, &ts->client->dev, "%s: w:%d, h:%d, x:%d, y:%d\n",
			__func__, rect_data[0], rect_data[1], rect_data[2], rect_data[3]);

	snprintf(buff, sizeof(buff), "%s", "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
	return;
NG:
	enable_irq(ts->client->irq);
	snprintf(buff, sizeof(buff), "%s", "NG");
	sec->cmd_state = SEC_CMD_STATUS_FAIL;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
}

static void aod_enable(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1)
		goto NG;

	if (sec->cmd_param[0]) {
		if (ts->use_sponge)
			ts->lowpower_mode |= SEC_TS_MODE_SPONGE_AOD;
	} else {
		if (ts->use_sponge)
			ts->lowpower_mode &= ~SEC_TS_MODE_SPONGE_AOD;
	}

	input_info(true, &ts->client->dev, "%s: %02X\n", __func__, ts->lowpower_mode);

	if (ts->use_sponge)
		sec_ts_set_custom_library(ts);

	snprintf(buff, sizeof(buff), "%s", "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
	return;

NG:
	snprintf(buff, sizeof(buff), "%s", "NG");
	sec->cmd_state = SEC_CMD_STATUS_FAIL;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
}

/*
 *	flag     1  :  set edge handler
 *		2  :  set (portrait, normal) edge zone data
 *		4  :  set (portrait, normal) dead zone data
 *		8  :  set landscape mode data
 *		16 :  mode clear
 *	data
 *		0x30, FFF (y start), FFF (y end),  FF(direction)
 *		0x31, FFFF (edge zone)
 *		0x32, FF (up x), FF (down x), FFFF (y)
 *		0x33, FF (mode), FFF (edge), FFF (dead zone)
 *	case
 *		edge handler set :  0x30....
 *		booting time :  0x30...  + 0x31...
 *		normal mode : 0x32...  (+0x31...)
 *		landscape mode : 0x33...
 *		landscape -> normal (if same with old data) : 0x33, 0
 *		landscape -> normal (etc) : 0x32....  + 0x33, 0
 */

void set_grip_data_to_ic(struct sec_ts_data *ts, u8 flag)
{
	u8 data[8] = { 0 };

	input_info(true, &ts->client->dev, "%s: flag: %02X (clr,lan,nor,edg,han)\n", __func__, flag);

	if (flag & G_SET_EDGE_HANDLER) {
		if (ts->grip_edgehandler_direction == 0) {
			data[0] = 0x0;
			data[1] = 0x0;
			data[2] = 0x0;
			data[3] = 0x0;
		} else {
			data[0] = (ts->grip_edgehandler_start_y >> 4) & 0xFF;
			data[1] = (ts->grip_edgehandler_start_y << 4 & 0xF0) | ((ts->grip_edgehandler_end_y >> 8) & 0xF);
			data[2] = ts->grip_edgehandler_end_y & 0xFF;
			data[3] = ts->grip_edgehandler_direction & 0x3;
		}
		ts->sec_ts_i2c_write(ts, SEC_TS_CMD_EDGE_HANDLER, data, 4);
		input_info(true, &ts->client->dev, "%s: 0x%02X %02X,%02X,%02X,%02X\n",
				__func__, SEC_TS_CMD_EDGE_HANDLER, data[0], data[1], data[2], data[3]);
	}

	if (flag & G_SET_EDGE_ZONE) {
		data[0] = (ts->grip_edge_range >> 8) & 0xFF;
		data[1] = ts->grip_edge_range  & 0xFF;
		ts->sec_ts_i2c_write(ts, SEC_TS_CMD_EDGE_AREA, data, 2);
		input_info(true, &ts->client->dev, "%s: 0x%02X %02X,%02X\n",
				__func__, SEC_TS_CMD_EDGE_AREA, data[0], data[1]);
	}

	if (flag & G_SET_NORMAL_MODE) {
		data[0] = ts->grip_deadzone_up_x & 0xFF;
		data[1] = ts->grip_deadzone_dn_x & 0xFF;
		data[2] = (ts->grip_deadzone_y >> 8) & 0xFF;
		data[3] = ts->grip_deadzone_y & 0xFF;
		ts->sec_ts_i2c_write(ts, SEC_TS_CMD_DEAD_ZONE, data, 4);
		input_info(true, &ts->client->dev, "%s: 0x%02X %02X,%02X,%02X,%02X\n",
				__func__, SEC_TS_CMD_DEAD_ZONE, data[0], data[1], data[2], data[3]);
	}

	if (flag & G_SET_LANDSCAPE_MODE) {
		data[0] = ts->grip_landscape_mode & 0x1;
		data[1] = (ts->grip_landscape_edge >> 4) & 0xFF;
		data[2] = (ts->grip_landscape_edge << 4 & 0xF0) | ((ts->grip_landscape_deadzone >> 8) & 0xF);
		data[3] = ts->grip_landscape_deadzone & 0xFF;
		ts->sec_ts_i2c_write(ts, SEC_TS_CMD_LANDSCAPE_MODE, data, 4);
		input_info(true, &ts->client->dev, "%s: 0x%02X %02X,%02X,%02X,%02X\n",
				__func__, SEC_TS_CMD_LANDSCAPE_MODE, data[0], data[1], data[2], data[3]);
	}

	if (flag & G_CLR_LANDSCAPE_MODE) {
		data[0] = ts->grip_landscape_mode;
		ts->sec_ts_i2c_write(ts, SEC_TS_CMD_LANDSCAPE_MODE, data, 1);
		input_info(true, &ts->client->dev, "%s: 0x%02X %02X\n",
				__func__, SEC_TS_CMD_LANDSCAPE_MODE, data[0]);
	}
}

/*
 *	index  0 :  set edge handler
 *		1 :  portrait (normal) mode
 *		2 :  landscape mode
 *
 *	data
 *		0, X (direction), X (y start), X (y end)
 *		direction : 0 (off), 1 (left), 2 (right)
 *			ex) echo set_grip_data,0,2,600,900 > cmd
 *
 *		1, X (edge zone), X (dead zone up x), X (dead zone down x), X (dead zone y)
 *			ex) echo set_grip_data,1,200,10,50,1500 > cmd
 *
 *		2, 1 (landscape mode), X (edge zone), X (dead zone)
 *			ex) echo set_grip_data,2,1,200,100 > cmd
 *
 *		2, 0 (portrait mode)
 *			ex) echo set_grip_data,2,0  > cmd
 */

static void set_grip_data(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	u8 mode = G_NONE;

	sec_cmd_set_default_result(sec);

	memset(buff, 0, sizeof(buff));

	mutex_lock(&ts->device_mutex);

	if (sec->cmd_param[0] == 0) {	// edge handler
		if (sec->cmd_param[1] == 0) {	// clear
			ts->grip_edgehandler_direction = 0;
		} else if (sec->cmd_param[1] < 3) {
			ts->grip_edgehandler_direction = sec->cmd_param[1];
			ts->grip_edgehandler_start_y = sec->cmd_param[2];
			ts->grip_edgehandler_end_y = sec->cmd_param[3];
		} else {
			input_err(true, &ts->client->dev, "%s: cmd1 is abnormal, %d (%d)\n",
					__func__, sec->cmd_param[1], __LINE__);
			goto err_grip_data;
		}

		mode = mode | G_SET_EDGE_HANDLER;
		set_grip_data_to_ic(ts, mode);

	} else if (sec->cmd_param[0] == 1) {	// normal mode
		if (ts->grip_edge_range != sec->cmd_param[1])
			mode = mode | G_SET_EDGE_ZONE;

		ts->grip_edge_range = sec->cmd_param[1];
		ts->grip_deadzone_up_x = sec->cmd_param[2];
		ts->grip_deadzone_dn_x = sec->cmd_param[3];
		ts->grip_deadzone_y = sec->cmd_param[4];
		mode = mode | G_SET_NORMAL_MODE;

		if (ts->grip_landscape_mode == 1) {
			ts->grip_landscape_mode = 0;
			mode = mode | G_CLR_LANDSCAPE_MODE;
		}
		set_grip_data_to_ic(ts, mode);
	} else if (sec->cmd_param[0] == 2) {	// landscape mode
		if (sec->cmd_param[1] == 0) {	// normal mode
			ts->grip_landscape_mode = 0;
			mode = mode | G_CLR_LANDSCAPE_MODE;
		} else if (sec->cmd_param[1] == 1) {
			ts->grip_landscape_mode = 1;
			ts->grip_landscape_edge = sec->cmd_param[2];
			ts->grip_landscape_deadzone	= sec->cmd_param[3];
			mode = mode | G_SET_LANDSCAPE_MODE;
		} else {
			input_err(true, &ts->client->dev, "%s: cmd1 is abnormal, %d (%d)\n",
					__func__, sec->cmd_param[1], __LINE__);
			goto err_grip_data;
		}
		set_grip_data_to_ic(ts, mode);
	} else {
		input_err(true, &ts->client->dev, "%s: cmd0 is abnormal, %d", __func__, sec->cmd_param[0]);
		goto err_grip_data;
	}

	mutex_unlock(&ts->device_mutex);

	snprintf(buff, sizeof(buff), "%s", "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
	return;

err_grip_data:
	mutex_unlock(&ts->device_mutex);

	snprintf(buff, sizeof(buff), "%s", "NG");
	sec->cmd_state = SEC_CMD_STATUS_FAIL;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
}

/*
 * Set/Get Dex Mode 0xE7
 *  0: Disable dex mode
 *  1: Full screen mode
 *  2: Iris mode
 */
static void dex_enable(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	int ret;

	sec_cmd_set_default_result(sec);

	if (!ts->plat_data->support_dex) {
		input_err(true, &ts->client->dev, "%s: not support DeX mode\n", __func__);
		goto NG;
	}

	if ((sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1) &&
			(sec->cmd_param[1] < 0 || sec->cmd_param[1] > 1)) {
		input_err(true, &ts->client->dev, "%s: not support param\n", __func__);
		goto NG;
	}

	sec_ts_unlocked_release_all_finger(ts);

	ts->dex_mode = sec->cmd_param[0];
	if (ts->dex_mode) {
		input_err(true, &ts->client->dev, "%s: set DeX touch_pad mode%s\n",
				__func__, sec->cmd_param[1] ? " & Iris mode" : "");
		ts->input_dev = ts->input_dev_pad;
		if (sec->cmd_param[1]) {
			/* Iris mode */
			ts->dex_mode = 0x02;
			ts->dex_name = "[DeXI]";
		} else {
			ts->dex_name = "[DeX]";
		}
	} else {
		input_err(true, &ts->client->dev, "%s: set touch mode\n", __func__);
		ts->input_dev = ts->input_dev_touch;
		ts->dex_name = "";
	}

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: Touch is stopped!\n", __func__);
		goto NG;
	}

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_DEX_MODE, &ts->dex_mode, 1);
	if (ret < 0) {
		input_err(true, &ts->client->dev,
				"%s: failed to set dex %smode\n", __func__,
				sec->cmd_param[1] ? "iris " : "");
		goto NG;
	}

	snprintf(buff, sizeof(buff), "%s", "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
	return;

NG:
	snprintf(buff, sizeof(buff), "%s", "NG");
	sec->cmd_state = SEC_CMD_STATUS_FAIL;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
}

static void brush_enable(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	int ret;

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1) {
		snprintf(buff, sizeof(buff), "%s", "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		goto out;
	}

	ts->brush_mode = sec->cmd_param[0];

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: Touch is stopped!\n", __func__);
		snprintf(buff, sizeof(buff), "%s", "TSP turned off");
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		goto out;
	}

	input_info(true, &ts->client->dev,
			"%s: set brush mode %s\n", __func__, ts->brush_mode ? "enable" : "disable");

	/*	- 0: Disable Artcanvas min phi mode
	 *	- 1: Enable Artcanvas min phi mode
	 */
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_BRUSH_MODE, &ts->brush_mode, 1);
	if (ret < 0) {
		input_err(true, &ts->client->dev,
				"%s: failed to set brush mode\n", __func__);
		snprintf(buff, sizeof(buff), "%s", "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		goto out;
	}

	snprintf(buff, sizeof(buff), "%s", "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;
out:
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void set_touchable_area(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	int ret;

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1) {
		snprintf(buff, sizeof(buff), "%s", "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		goto out;
	}

	ts->touchable_area = sec->cmd_param[0];

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: Touch is stopped!\n", __func__);
		snprintf(buff, sizeof(buff), "%s", "TSP turned off");
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		goto out;
	}

	input_info(true, &ts->client->dev,
			"%s: set 16:9 mode %s\n", __func__, ts->touchable_area ? "enable" : "disable");

	/*  - 0: Disable 16:9 mode
	 *  - 1: Enable 16:9 mode
	 */
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_TOUCHABLE_AREA, &ts->touchable_area, 1);
	if (ret < 0) {
		input_err(true, &ts->client->dev,
				"%s: failed to set 16:9 mode\n", __func__);
		snprintf(buff, sizeof(buff), "%s", "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		goto out;
	}

	snprintf(buff, sizeof(buff), "%s", "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;
out:
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void set_log_level(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	char tBuff[2] = { 0 };
	u8 w_data[1] = {0x00};
	int ret;

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: Touch is stopped!\n", __func__);
		snprintf(buff, sizeof(buff), "%s", "TSP turned off");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		return;
	}

	if ((sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1) ||
			(sec->cmd_param[1] < 0 || sec->cmd_param[1] > 1) ||
			(sec->cmd_param[2] < 0 || sec->cmd_param[2] > 1) ||
			(sec->cmd_param[3] < 0 || sec->cmd_param[3] > 1) ||
			(sec->cmd_param[4] < 0 || sec->cmd_param[4] > 1) ||
			(sec->cmd_param[5] < 0 || sec->cmd_param[5] > 1) ||
			(sec->cmd_param[6] < 0 || sec->cmd_param[6] > 1) ||
			(sec->cmd_param[7] < 0 || sec->cmd_param[7] > 1)) {
		input_err(true, &ts->client->dev, "%s: para out of range\n", __func__);
		snprintf(buff, sizeof(buff), "%s", "Para out of range");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		return;
	}

	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_STATUS_EVENT_TYPE, tBuff, 2);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: Read Event type enable status fail\n", __func__);
		snprintf(buff, sizeof(buff), "%s", "Read Stat Fail");
		goto err;
	}

	input_info(true, &ts->client->dev, "%s: STATUS_EVENT enable = 0x%02X, 0x%02X\n",
			__func__, tBuff[0], tBuff[1]);

	tBuff[0] = BIT_STATUS_EVENT_VENDOR_INFO(sec->cmd_param[6]);
	tBuff[1] = BIT_STATUS_EVENT_ERR(sec->cmd_param[0]) |
		BIT_STATUS_EVENT_INFO(sec->cmd_param[1]) |
		BIT_STATUS_EVENT_USER_INPUT(sec->cmd_param[2]);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_STATUS_EVENT_TYPE, tBuff, 2);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: Write Event type enable status fail\n", __func__);
		snprintf(buff, sizeof(buff), "%s", "Write Stat Fail");
		goto err;
	}

	if (sec->cmd_param[0] == 1 && sec->cmd_param[1] == 1 &&
			sec->cmd_param[2] == 1 && sec->cmd_param[3] == 1 &&
			sec->cmd_param[4] == 1 && sec->cmd_param[5] == 1 &&
			sec->cmd_param[6] == 1 && sec->cmd_param[7] == 1) {
		w_data[0] = 0x1;
		ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_VENDOR_EVENT_LEVEL, w_data, 1);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: Write Vendor Event Level fail\n", __func__);
			snprintf(buff, sizeof(buff), "%s", "Write Stat Fail");
			goto err;
		}
	} else {
		w_data[0] = 0x0;
		ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_VENDOR_EVENT_LEVEL, w_data, 0);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: Write Vendor Event Level fail\n", __func__);
			snprintf(buff, sizeof(buff), "%s", "Write Stat Fail");
			goto err;
		}
	}

	input_info(true, &ts->client->dev, "%s: ERROR : %d, INFO : %d, USER_INPUT : %d, INFO_SPONGE : %d, VENDOR_INFO : %d, VENDOR_EVENT_LEVEL : %d\n",
			__func__, sec->cmd_param[0], sec->cmd_param[1], sec->cmd_param[2], sec->cmd_param[5], sec->cmd_param[6], w_data[0]);

	snprintf(buff, sizeof(buff), "%s", "OK");
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	return;
err:
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
}

static void debug(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };

	sec_cmd_set_default_result(sec);

	ts->debug_flag = sec->cmd_param[0];

	snprintf(buff, sizeof(buff), "%s", "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
}

static void check_connection(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = {0};
	int rc;
	int size = SEC_TS_SELFTEST_REPORT_SIZE + ts->tx_count * ts->rx_count * 2;
	u8 *rBuff = NULL;
	int ii;
	u8 data[8] = { 0 };
	int result = 0;

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: Touch is stopped!\n", __func__);
		snprintf(buff, sizeof(buff), "%s", "TSP turned off");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		return;
	}

	rBuff = kzalloc(size, GFP_KERNEL);
	if (!rBuff) {
		snprintf(buff, sizeof(buff), "%s", "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		return;
	}

	memset(rBuff, 0x00, size);
	memset(data, 0x00, 8);

	disable_irq(ts->client->irq);

	sec_ts_locked_release_all_finger(ts);

	input_info(true, &ts->client->dev, "%s: set power mode to test mode\n", __func__);
	data[0] = 0x02;
	rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, data, 1);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: set test mode failed\n", __func__);
		goto err_conn_check;
	}

	input_info(true, &ts->client->dev, "%s: clear event stack\n", __func__);
	rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_CLEAR_EVENT_STACK, NULL, 0);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: clear event stack failed\n", __func__);
		goto err_conn_check;
	}

	data[0] = 0xB7;
	input_info(true, &ts->client->dev, "%s: self test start\n", __func__);
	rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SELFTEST, data, 1);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: Send selftest cmd failed!\n", __func__);
		goto err_conn_check;
	}

	sec_ts_delay(700);

	rc = sec_ts_wait_for_ready(ts, SEC_TS_VENDOR_ACK_SELF_TEST_DONE);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: Selftest execution time out!\n", __func__);
		goto err_conn_check;
	}

	input_info(true, &ts->client->dev, "%s: self test done\n", __func__);
	rc = ts->sec_ts_i2c_read(ts, SEC_TS_READ_SELFTEST_RESULT, rBuff, size);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: Selftest execution time out!\n", __func__);
		goto err_conn_check;
	}

	data[0] = TO_TOUCH_MODE;
	rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, data, 1);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: mode changed failed!\n", __func__);
		goto err_conn_check;
	}

	input_info(true, &ts->client->dev, "%s: %02X, %02X, %02X, %02X\n",
			__func__, rBuff[16], rBuff[17], rBuff[18], rBuff[19]);

	memcpy(data, &rBuff[48], 8);

	for (ii = 0; ii < 8; ii++) {
		input_info(true, &ts->client->dev, "%s: [%d] %02X\n", __func__, ii, data[ii]);

		result += data[ii];
	}

	if (result != 0)
		goto err_conn_check;

	enable_irq(ts->client->irq);

	snprintf(buff, sizeof(buff), "%s", "OK");
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;

	kfree(rBuff);
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
	return;

err_conn_check:
	data[0] = TO_TOUCH_MODE;
	ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, data, 1);
	enable_irq(ts->client->irq);

	snprintf(buff, sizeof(buff), "%s", "NG");
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_FAIL;

	kfree(rBuff);
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void not_support_cmd(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	char buff[SEC_CMD_STR_LEN] = { 0 };

	sec_cmd_set_default_result(sec);
	snprintf(buff, sizeof(buff), "%s", "NA");

	sec_cmd_set_cmd_result(sec, buff, strlen(buff));
	sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
	sec_cmd_set_cmd_exit(sec);
}

int sec_ts_fn_init(struct sec_ts_data *ts)
{
	int retval;

	retval = sec_cmd_init(&ts->sec, sec_cmds,
			ARRAY_SIZE(sec_cmds), SEC_CLASS_DEVT_TSP);
	if (retval < 0) {
		input_err(true, &ts->client->dev,
				"%s: Failed to sec_cmd_init\n", __func__);
		goto exit;
	}

	retval = sysfs_create_group(&ts->sec.fac_dev->kobj,
			&cmd_attr_group);
	if (retval < 0) {
		input_err(true, &ts->client->dev,
				"%s: FTS Failed to create sysfs attributes\n", __func__);
		goto exit;
	}

	retval = sysfs_create_link(&ts->sec.fac_dev->kobj,
			&ts->input_dev->dev.kobj, "input");
	if (retval < 0) {
		input_err(true, &ts->client->dev,
				"%s: Failed to create input symbolic link\n",
				__func__);
		goto exit;
	}

	ts->reinit_done = true;

	return 0;

exit:
	return retval;
}

void sec_ts_fn_remove(struct sec_ts_data *ts)
{
	input_err(true, &ts->client->dev, "%s\n", __func__);

	sysfs_delete_link(&ts->sec.fac_dev->kobj, &ts->input_dev->dev.kobj, "input");

	sysfs_remove_group(&ts->sec.fac_dev->kobj,
			&cmd_attr_group);

	sec_cmd_exit(&ts->sec, SEC_CLASS_DEVT_TSP);

}
