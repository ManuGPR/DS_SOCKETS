#!/bin/bash

test/test_init
test/test_set_value
test/test_get_value
test/test_modify_value
test/test_delete_key
test/test_exist
test/test_concurrencia &
test/test_concurrencia &
test/test_concurrencia &

