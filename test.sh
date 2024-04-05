#!/bin/bash

GREEN='\033[1;32m'
NC='\033[0m'

echo -e "${GREEN}======Test de init======${NC}"
test/test_init
echo -e "${GREEN}======Test de set_value======${NC}"
test/test_set_value
echo -e "${GREEN}======Test de get_value======${NC}"
test/test_get_value
echo -e "${GREEN}======Test de modify_value======${NC}"
test/test_modify_value
echo -e "${GREEN}======Test de delete_key======${NC}"
test/test_delete_key
echo -e "${GREEN}======Test de exist======${NC}"
test/test_exist
echo -e "${GREEN}======Test de concurrencia=====${NC}"
test/test_concurrencia &
test/test_concurrencia &
test/test_concurrencia &

