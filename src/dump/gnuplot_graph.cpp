#include "gnuplot_graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../io/output_expr_to_file.h"
#include "../calculation_optimization/calcul_tree.h"
#include <assert.h>

static char* CreateDumpFile(const char* format);

static TreeErr_t PrintInfo(Forest_t *forest1, Forest_t *forest2, size_t idx1, size_t idx2, FILE* gp_dump, const char* svg_filename);

static void MakePicture(const char* gp_filename, TreeErr_t* err);

char* DrawGraph(Forest_t *diff_forest, Forest_t *forest_taylor, size_t idx1, size_t idx2, TreeErr_t *err){
    DEBUG_TREE( *err = TreeNodeVerify(diff_forest->head_arr[idx1]->root);
                *err = TreeNodeVerify(forest_taylor->head_arr[idx2]->root);)
    if(*err) return NULL;

    char* svg_filename = CreateDumpFile("svg");
    char* gp_filename = CreateDumpFile("gp");
    if(!svg_filename || !gp_filename){
        *err = CANT_CREATE_FILE_FOR_GP_DUMP;
        return NULL;
    }

    FILE* gp_dump = fopen(gp_filename, "w");
    if(!gp_dump){
        *err = CANT_OPEN_OUT_FILE;
        free(gp_filename);
        free(svg_filename);
        return NULL;
    }

    *err = PrintInfo(diff_forest, forest_taylor, idx1, idx2, gp_dump, svg_filename);
    if(*err){
        free(gp_filename);
        free(svg_filename);
        return NULL;
    }

    fclose(gp_dump);

    MakePicture(gp_filename, err);
    if(*err){
        free(gp_filename);
        free(svg_filename);
        return NULL;
    }

    free(gp_filename);

    DEBUG_TREE( *err = TreeNodeVerify(diff_forest->head_arr[idx1]->root);
                *err = TreeNodeVerify(forest_taylor->head_arr[idx2]->root);)
    return svg_filename;
}

static char* CreateDumpFile(const char* format){
    static int num = 0;
    num++;

    if(!strncmp(format, "gp", 2) && !strncmp(format, "svg", 3)){
        fprintf(stderr, "Incorrect format - need gp or svg\n");
        return NULL;
    }

    char* svg_filename = (char*)calloc(200, sizeof(char));
    if(!svg_filename){
        fprintf(stderr, "Alloc error");
        return NULL;
    }

    time_t now = time(NULL); 
    struct tm *t = localtime(&now); 

    if(snprintf(svg_filename, 200, 
                "output/plots_gnuplot/dump%d_%04d%02d%02d_%02d%02d%02d.%s", num,
                t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                t->tm_hour, t->tm_min, t->tm_sec, format) == -1) {
        fprintf(stderr, "Can't generate svg filename\n");
        free(svg_filename);
        return NULL;
    }
    return svg_filename;
}

static TreeErr_t PrintInfo(Forest_t *forest1, Forest_t *forest2, size_t idx1, size_t idx2, FILE* gp_dump, const char* svg_filename){
    double delta = 10;
    double min_value_x = forest1->mtk->var_info[0].value - delta;
    double max_value_x = forest1->mtk->var_info[0].value + delta;
    char var_name = forest1->mtk->var_info[0].variable_name;

    fprintf(gp_dump, 
    "set terminal svg\n"
    "set output '%s'\n"
    "set xlabel \"x\"\n"
    "set ylabel \"y\"\n"
    "set grid\n"

    "set xrange [%lg:%lg]\n"

    "set key top left\n"            
    "set key box\n"               
    "set key spacing 2.5\n"
    "set key width 2.5\n"
    "set key height 1\n", svg_filename, min_value_x, max_value_x);  

    fprintf(gp_dump, "f(%c) = ", var_name);
    CHECK_AND_RET_TREEERR(DumpToFile(gp_dump, forest1->head_arr[idx1]->root, forest1->mtk));
    fprintf(gp_dump, "\n");

    if(forest1 == forest2){
        fprintf(gp_dump, "df(%c) = ", var_name);
        CHECK_AND_RET_TREEERR(DumpToFile(gp_dump, forest2->head_arr[idx2]->root, forest1->mtk));
    }
    else{   
        fprintf(gp_dump, "Tf(%c) = ", var_name);
        CHECK_AND_RET_TREEERR(DumpToFileTaylor(gp_dump, forest2, forest1));
    }

    fprintf(gp_dump, "\n");

    fprintf(gp_dump, "plot f(%c) with lines linecolor \"blue\" title \"f(%c)\", \\\n", var_name, var_name);
    if(forest1 == forest2){
        fprintf(gp_dump, "df(%c) with lines linecolor \"red\" title \"f'(%c)\" \n", var_name, var_name);
    }
    else{
        fprintf(gp_dump, "Tf(%c) with lines linecolor \"red\" title \"T(%c)\" \n", var_name, var_name);
    }
    
    return NO_MISTAKE_T;
}

static void MakePicture(const char* gp_filename, TreeErr_t* err){
    char sys_buffer[300] = {};

    strncpy(sys_buffer, "gnuplot ", sizeof("gnuplot "));
    strncat(sys_buffer, gp_filename, 100);

    if(system(sys_buffer)){;
        *err = CANT_MAKE_GRAPH;
    }
}