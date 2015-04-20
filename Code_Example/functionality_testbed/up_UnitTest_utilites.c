//
//  up_UnitTest_utilites.c
//
//  Created by o_0 on 2015-04-19.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include "up_UnitTest_utilites.h"
#include "up_error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



////////////////////// unit test  vec3_list /////////////////////////


static int unitTest_vec3_equal(struct up_vec3 *vecA,struct up_vec3 *vecB)
{
    
    return ((vecA->x == vecB->x) && (vecA->y == vecB->y) && (vecA->z == vecB->z));
}

static void unitTest_vec3_list_equal(struct up_vec3 *test_data,struct up_generic_list * list, int test_size)
{
    int i = 0;
    struct up_vec3 tmp_vec = {0};
    for (i = 0; i< test_size; i++) {
        up_vec3_list_getAtIndex(list, &tmp_vec, i);
        if (unitTest_vec3_equal(&test_data[i],&tmp_vec)) {
            printf(".");
        }else{
            printf("x");
        }
    }
}

void up_unitTest_list()
{
    int test_size = 500;
    
    srand((unsigned int)time(NULL));
    
    struct up_vec3 *test_data = malloc(sizeof(struct up_vec3) * test_size);
    if (test_data == NULL) {
        UP_ERROR_MSG("malloc failed");
        return ;
    }
    int i = 0;
    for (i = 0; i<test_size; i++) {
        test_data[i].x = rand()%1024;
        test_data[i].y = rand()%1024;
        test_data[i].z = rand()%1024;
    }
    printf("\nChecking list add :");
    struct up_generic_list * list = up_vec3_list_new(test_size % 20 + 4);
    for (i = 0; i < test_size; i++) {
        up_vec3_list_add(list, &test_data[i]);
    }
    
    //struct up_vec3 tmp_vec = {0};
    printf("\nChecking list count :");
    if(up_vec3_list_count(list) != test_size)
    {
        printf("\n\tFailed test");
    }
    printf("\nlist count completed");
    
    
    // check if they have the same data
    //struct up_vec3 tmp_vec = {0};
    printf("\nChecking list get integrety :");
    unitTest_vec3_list_equal(test_data, list, test_size);
    printf("\nlist get integrety completed");
    printf("\nChecking list add completed\n");
    
    
    printf("\nChecking list set integrety :");
    
    int index = 0;
    for (i = 0; i < 50; i++) {
        index = rand() % test_size;
        test_data[index].x = rand()%1024;
        test_data[index].y = rand()%1024;
        test_data[index].z = rand()%1024;
        up_vec3_list_setAtIndex(list, &test_data[index], index);
        
    }
    unitTest_vec3_list_equal(test_data, list, test_size);
    printf("\nlist set integrety completed\n");
    
    up_vec3_list_delete(list);
    
    printf("\nChecking list transferOwnership:");
    struct up_generic_list * list2 = up_vec3_list_new(test_size % 20 + 4);
    for (i = 0; i < test_size; i++) {
        up_vec3_list_add(list2, &test_data[i]);
    }
    
    struct up_vec3 *test_data2 = up_vec3_list_transferOwnership(&list2);
    if (list2 != NULL) {
        printf("\n\tlist failed to be set to NULL\n");
        up_vec3_list_delete(list2);
        free(test_data);
        return ;
    }
    
    if (test_data2 == NULL) {
        printf("\n\townership failed to be returnd \n");
        free(test_data);
        return ;
    }
    printf("\nlist stage1 ownership transefer completed\n");
    printf("Checking data integrety :");
    for (i = 0; i< test_size; i++) {
        if (unitTest_vec3_equal(&test_data[i],&test_data2[i])) {
            printf(".");
        }else{
            printf("x");
        }
    }
    printf("\ndata integrety completed\n");
    printf("\nlist transferOwnership: Completed");
    free(test_data2);
    free(test_data);
}