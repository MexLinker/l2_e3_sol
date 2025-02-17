#include <zephyr/types.h>
#include <stddef.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/addr.h>  // Required for address utilities

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zephyr/bluetooth/gap.h>
/* STEP 3.2.1 - Include the header file of the UUID helper macros and definitions */
#include <zephyr/bluetooth/uuid.h>

#include <dk_buttons_and_leds.h>

static const struct bt_le_adv_param *adv_param = BT_LE_ADV_PARAM(
	(BT_LE_ADV_OPT_CONNECTABLE |
	 BT_LE_ADV_OPT_USE_IDENTITY), /* Connectable advertising and use identity address */
	800, /* Min Advertising Interval 500ms (800*0.625ms) */
	801, /* Max Advertising Interval 500.625ms (801*0.625ms) */
	NULL); /* Set to NULL for undirected advertising */


#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

#define RUN_STATUS_LED DK_LED1
#define RUN_LED_BLINK_INTERVAL 1000

static uint8_t mfg_data[] = { 0xff, 0xff, 0x00 };

static const struct bt_data ad[] = {
	/* STEP 3.1 - Set the flags and populate the device name in the advertising packet */
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
	BT_DATA(BT_DATA_MANUFACTURER_DATA, mfg_data, 3),  // Existing manufacturer data

};




// // Device name to advertise
// static const char device_name[] = "MyBLEDevice";

// // static const struct bt_data ad[] = {
// // 	BT_DATA(BT_DATA_MANUFACTURER_DATA, mfg_data, 3),
// // };

// static const struct bt_data ad[] = {
//     BT_DATA(BT_DATA_MANUFACTURER_DATA, mfg_data, 3),  // Existing manufacturer data
//     BT_DATA(BT_DATA_NAME_COMPLETE, device_name, sizeof(device_name) - 1),  // Add device name
// };

static void scan_cb(const bt_addr_le_t *addr, int8_t rssi, uint8_t adv_type,
		    struct net_buf_simple *buf)
{
	char addr_str[BT_ADDR_LE_STR_LEN];  // Buffer for address string

	// Convert BLE address to string and print
	bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
	printk("Scanned Device: Address=%s, RSSI=%d, Type=%u\n",
	       addr_str, rssi, adv_type);

	mfg_data[2]++;
}

int main(void)
{
	struct bt_le_scan_param scan_param = {
		.type       = BT_HCI_LE_SCAN_PASSIVE,
		.options    = BT_LE_SCAN_OPT_NONE,
		.interval   = 0x0010,
		.window     = 0x0010,
	};
	int err;

	printk("Starting Scanner/Advertiser Demo\n");


	bt_addr_le_t addr;
	err = bt_addr_le_from_str("FF:EE:DD:CC:BB:AA", "random", &addr);
	err = bt_id_create(&addr, NULL);


	err = bt_enable(NULL);

	// if (err) {
	// 	printk("Bluetooth init failed (err %d)\n", err);
	// 	return 0;
	// }

	// printk("Bluetooth initialized\n");

	// err = bt_le_scan_start(&scan_param, scan_cb);
	// if (err) {
	// 	printk("Starting scanning failed (err %d)\n", err);
	// 	return 0;
	// }

	do {

		k_sleep(K_MSEC(400));

		err = bt_le_adv_start(adv_param, ad, ARRAY_SIZE(ad),
				      NULL, 0);

		k_sleep(K_MSEC(40));

		err = bt_le_adv_stop();

		k_sleep(K_MSEC(400));



		k_sleep(K_MSEC(400));

		err = bt_le_scan_start(&scan_param, scan_cb);


		k_sleep(K_MSEC(40));

		err = bt_le_scan_stop();

		k_sleep(K_MSEC(400));


	} while (1);
	return 0;
}