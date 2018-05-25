#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

/* function && variable rename */
#define get_inode mock_get_inode
#define read_sector mock_read_sector

#include "../../prog/fs/path.c"

/* variable mock */


/* function mock */
int counter_mock_get_inode = 0;
int counter_mock_read_sector = 0;
INode* mock_get_inode(uint32_t dev, int nr_inode){
    counter_mock_get_inode++;
    return (INode*)(unsigned int)mock();
}
uint8_t* mock_src_buf[SECTOR_SIZE + 1];
void mock_read_sector(uint32_t sector, void* buf, uint32_t len){
    memcpy(buf, mock_src_buf, sizeof(uint8_t) * (SECTOR_SIZE));
    counter_mock_read_sector++;
}


/* unit test */
static void test_get_file_inode_success(void **state){
    INode folder_inode;
    folder_inode.access_mode = ACCESS_MODE_DIRECTORY;
    folder_inode.file_size = 10;
    Dir_Entry* dp = (Dir_Entry*)mock_src_buf;
    dp[0].file_name[0] = 'a';
    dp[0].nr_inode = 10;
    will_return_always(mock_get_inode, &folder_inode);
    assert_int_equal(get_file_inode(1, "a", 1), 10);
}

static void test_search_file(void **state){
    INode folder_inode;
    folder_inode.access_mode = ACCESS_MODE_DIRECTORY;
    folder_inode.file_size = 10;
    Dir_Entry* dp = (Dir_Entry*)mock_src_buf;
    dp[0].file_name[0] = 'a';
    dp[0].nr_inode = 10;
    will_return_always(mock_get_inode, &folder_inode);
    assert_int_equal(search_file("/a"), 10);
}

int main(int argc, char* argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_get_file_inode_success),
        cmocka_unit_test(test_search_file)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
