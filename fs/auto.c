#include <linux/module.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>
#include <linux/blkdev.h>
#include <linux/mount.h>
#include <linux/string.h>
#include <linux/slab.h>

#define EROFS_MAGIC     0xE0F5E1E2
#define EXT4_SUPER_MAGIC 0xEF53
#define EROFS_OFFSET    1024
#define EXT4_SB_OFFSET  1024

static int detect_filesystem_type(const char *dev_name, char *fs_name, size_t len)
{
    struct block_device *bdev;
    struct buffer_head *bh = NULL;
    fmode_t mode = FMODE_READ;
    __le32 erofs_magic;
    __le16 ext4_magic;
    int result = -1;

    bdev = blkdev_get_by_path(dev_name, mode, NULL);
    if (IS_ERR(bdev)) {
        printk(KERN_DEBUG "AUTO-FS: Cannot open %s for detection\n", dev_name);
        return -1;
    }

    bh = __bread(bdev, EROFS_OFFSET / 4096, 4096);
    if (bh) {
        erofs_magic = le32_to_cpu(*((__le32 *)(bh->b_data + (EROFS_OFFSET % 4096))));
        if (erofs_magic == EROFS_MAGIC) {
            strncpy(fs_name, "erofs", len - 1);
            fs_name[len - 1] = '\0';
            result = 0;
            printk(KERN_INFO "AUTO-FS: Detected EROFS on %s (magic=0x%08x)\n", 
                   dev_name, erofs_magic);
            brelse(bh);
            goto cleanup;
        }
        brelse(bh);
    }

    bh = __bread(bdev, EXT4_SB_OFFSET / 4096, 4096);
    if (bh) {
        ext4_magic = le16_to_cpu(*((__le16 *)(bh->b_data + (EXT4_SB_OFFSET % 4096) + 56)));
        if (ext4_magic == EXT4_SUPER_MAGIC) {
            strncpy(fs_name, "ext4", len - 1);
            fs_name[len - 1] = '\0';
            result = 0;
            printk(KERN_INFO "AUTO-FS: Detected EXT4 on %s (magic=0x%04x)\n", 
                   dev_name, ext4_magic);
        } else {
            strncpy(fs_name, "ext4", len - 1);
            fs_name[len - 1] = '\0';
            result = 0;
            printk(KERN_INFO "AUTO-FS: Unknown magic 0x%04x, defaulting to EXT4 for %s\n", 
                   ext4_magic, dev_name);
        }
        brelse(bh);
    } else {
        strncpy(fs_name, "ext4", len - 1);
        fs_name[len - 1] = '\0';
        result = 0;
        printk(KERN_INFO "AUTO-FS: Cannot read superblock, defaulting to EXT4 for %s\n", 
               dev_name);
    }

cleanup:
    blkdev_put(bdev, mode);
    return result;
}

static char *filter_erofs_options(const char *data)
{
    char *result, *pos;
    
    if (!data) return NULL;
    
    result = kstrdup(data, GFP_KERNEL);
    if (!result) return NULL;
    
    pos = strstr(result, "barrier=1");
    if (pos) {
        if (pos > result && *(pos-1) == ',') {
            memmove(pos-1, pos+9, strlen(pos+9)+1);
        } else if (*(pos+9) == ',') {
            memmove(pos, pos+10, strlen(pos+10)+1);
        } else {
            *pos = '\0';
        }
    }
    
    pos = strstr(result, "discard");
    if (pos) {
        if (pos > result && *(pos-1) == ',') {
            memmove(pos-1, pos+7, strlen(pos+7)+1);
        } else if (*(pos+7) == ',') {
            memmove(pos, pos+8, strlen(pos+8)+1);
        } else {
            *pos = '\0';
        }
    }
    return result;
}

static struct dentry *auto_mount(struct file_system_type *fs_type, int flags,
                                const char *dev_name, void *data)
{
    char detected_fs[16];
    struct file_system_type *target_fs;
    struct dentry *result;
    char *filtered_data = NULL;
    void *mount_data = data;

    if (detect_filesystem_type(dev_name, detected_fs, sizeof(detected_fs)) != 0) {
        printk(KERN_ERR "AUTO-FS: Failed to detect filesystem type on %s\n", dev_name);
        return ERR_PTR(-EINVAL);
    }

    /* Filter barrier=1 for EROFS */
    if (strcmp(detected_fs, "erofs") == 0 && data != NULL) {
        filtered_data = filter_erofs_options((char *)data);
        if (filtered_data) mount_data = filtered_data;
    }

    target_fs = get_fs_type(detected_fs);
    if (!target_fs) {
        printk(KERN_ERR "AUTO-FS: Filesystem driver '%s' not available for %s\n", 
               detected_fs, dev_name);
        if (filtered_data) kfree(filtered_data);
        return ERR_PTR(-ENODEV);
    }

    printk(KERN_INFO "AUTO-FS: Mounting %s as %s\n", dev_name, detected_fs);
    result = target_fs->mount(target_fs, flags, dev_name, mount_data);
    put_filesystem(target_fs);

    if (!IS_ERR(result)) {
        printk(KERN_INFO "AUTO-FS: Successfully mounted %s as %s\n", 
               dev_name, detected_fs);
    } else {
        printk(KERN_ERR "AUTO-FS: Failed to mount %s as %s: %ld\n", 
               dev_name, detected_fs, PTR_ERR(result));
    }

    /* Clean up */
    if (filtered_data) kfree(filtered_data);

    return result;
}

static struct file_system_type auto_fs_type = {
    .owner      = THIS_MODULE,
    .name       = "auto",
    .mount      = auto_mount,
    .kill_sb    = kill_block_super,
    .fs_flags   = FS_REQUIRES_DEV,
};

static int __init auto_fs_init(void)
{
    int ret;

    ret = register_filesystem(&auto_fs_type);
    if (ret) {
        printk(KERN_ERR "AUTO-FS: Failed to register auto filesystem: %d\n", ret);
        return ret;
    }

    printk(KERN_INFO "AUTO-FS: Auto-detection filesystem registered successfully\n");
    return 0;
}

static void __exit auto_fs_exit(void)
{
    unregister_filesystem(&auto_fs_type);
    printk(KERN_INFO "AUTO-FS: Auto-detection filesystem unregistered\n");
}

module_init(auto_fs_init);
module_exit(auto_fs_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Auto-detecting filesystem for EROFS/EXT4");
MODULE_AUTHOR("Andrey");
MODULE_VERSION("1.2");