#include "gnuplot_graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../io/input_output.h"
#include "../calculation_optimization/calcul_tree.h"
#include <assert.h>
/*
короче идейно должны всегда рисовать график функции и график производной
Единственное у функции двух переменных мы не можем вставить график
*/

static char* CreateDumpFile(const char* format);

static TreeErr_t PrintInfo(TreeNode_t* node1, TreeNode_t* node2, metki* mtk, FILE* gp_dump, const char* svg_filename);

static void MakePicture(const char* gp_filename, TreeErr_t* err);

char* DrawGraph(TreeNode_t* node1, TreeNode_t* node2, metki* mtk, TreeErr_t *err){
    assert(node1); assert(node2); assert(mtk); assert(err);

    if(mtk->first_free != 1){
        fprintf(stderr, "Can't draw plot of 2 variables");
        *err =  NO_MISTAKE_T;
        return NULL;
    }

    DEBUG_TREE( *err = TreeNodeVerify(node1);
                *err = TreeNodeVerify(node2);)
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

    *err = PrintInfo(node1, node2, mtk, gp_dump, svg_filename);
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

    DEBUG_TREE( *err = TreeNodeVerify(node1);
                *err = TreeNodeVerify(node2);)
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

static TreeErr_t PrintInfo(TreeNode_t* node1, TreeNode_t* node2, metki* mtk, FILE* gp_dump, const char* svg_filename){
    double delta = 10;
    double min_value_x = mtk->var_info[0].value - delta;
    double max_value_x = mtk->var_info[0].value + delta;


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

    fprintf(gp_dump, "f(%c) = ", mtk->var_info[0].variable_name);
    CHECK_AND_RET_TREEERR(DumpToFile(gp_dump, node1, mtk, 0));
    fprintf(gp_dump, "\n");

    fprintf(gp_dump, "df(%c) = ", mtk->var_info[0].variable_name);
    CHECK_AND_RET_TREEERR(DumpToFile(gp_dump, node2, mtk, 0));
    fprintf(gp_dump, "\n");

    fprintf(gp_dump, 
    "plot f(x) with lines linecolor \"blue\" title \"f(x)\", \\\n"
        "df(x) with lines linecolor \"red\" title \"f'(x)\" \n"
    );
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