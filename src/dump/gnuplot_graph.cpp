#include "gnuplot_graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "../io/output_expr_to_file.h"
#include "../calculation_optimization/calcul_tree.h"
#include "../utils/check_sys.h"
#include <assert.h>

const char* const ALLOWED = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_\t\n /.";

const size_t MAX_FILE_NAME = 200;

#define CALL_FUNC_AND_CHECK_ERR(function)\
    do{\
        function;\
        if(*err){ \
            fprintf(stderr, "err = %llu, %s, %s, %d\n", *err, __FILE__, __func__, __LINE__); \
            return;                                                         \
        } \
    }while(0)

static char* CreateDumpFile(const char* format);

static void PrintInfo(Forest_t *forest, size_t idx1, size_t idx2, FILE* gp_dump, const char* svg_filename, bool is_taylor, TreeErr_t *err);

static void MakePicture(const char* gp_filename, TreeErr_t* err);

char* DrawGraph(Forest_t *forest, size_t idx1, TreeErr_t *err, bool is_taylor, size_t idx2){
    DEBUG_TREE(           *err = TreeNodeVerify(forest->head_arr[idx1]->root);
    if(is_taylor)         *err = TreeNodeVerify(forest->head_arr[idx2]->root);)
    if(*err) return NULL;

    char* svg_filename = CreateDumpFile("svg");
    char* gp_filename = CreateDumpFile("gp");
    if(!svg_filename || !gp_filename){
        if(gp_filename)  free(gp_filename);
        if(svg_filename) free(svg_filename);
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

    PrintInfo(forest, idx1, idx2, gp_dump, svg_filename, is_taylor, err);
    if(*err){
        free(gp_filename);
        free(svg_filename);
        fclose(gp_dump);
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

    DEBUG_TREE(   *err = TreeNodeVerify(forest->head_arr[idx1]->root);
    if(is_taylor) *err = TreeNodeVerify(forest->head_arr[idx2]->root);)
    return svg_filename;
}

static char* CreateDumpFile(const char* format){
    static int num = 0;
    num++;

    if(!strncmp(format, "gp", 2) && !strncmp(format, "svg", 3)){
        fprintf(stderr, "Incorrect format - need gp or svg\n");
        return NULL;
    }

    char* filename = (char*)calloc(MAX_FILE_NAME, sizeof(char));
    if(!filename){
        fprintf(stderr, "Alloc error");
        return NULL;
    }

    time_t now = time(NULL); 
    struct tm *t = localtime(&now); 

    if(snprintf(filename, MAX_FILE_NAME, 
                "output/plots_gnuplot/dump%d_%04d%02d%02d_%02d%02d%02d.%s", num,
                t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                t->tm_hour, t->tm_min, t->tm_sec, format) == -1) {
        fprintf(stderr, "Can't generate svg filename\n");
        free(filename);
        return NULL;
    }
    return filename;
}

static void PrintInfo(Forest_t *forest, size_t idx1, size_t idx2, FILE* gp_dump, const char* svg_filename, bool is_taylor, TreeErr_t *err){
    double delta = 3;
    double min_value_x = forest->x_y_range.x_min_dump;
    double max_value_x = forest->x_y_range.x_max_dump;
    double min_value_y = forest->x_y_range.y_min_dump;
    double max_value_y = forest->x_y_range.y_max_dump;
    if(is_taylor && (forest->mtk->var_info[0].value <= min_value_x || forest->mtk->var_info[0].value >= max_value_x)){
        min_value_x = forest->mtk->var_info[0].value - delta;
        max_value_x = forest->mtk->var_info[0].value + delta;
        min_value_y = 0;
        max_value_y = 0;
    }

    fprintf(gp_dump, 
    "set terminal svg\n"
    "set output '%s'\n"
    "set xlabel \"x\"\n"
    "set ylabel \"y\"\n"
    "set grid\n", svg_filename);

    // x_min should not equal x_max!
    if(fabs(min_value_x - max_value_x) > EPS){
        fprintf(gp_dump, "set xrange [%lg:%lg]\n", min_value_x, max_value_x);
    } 
    else{
        fprintf(gp_dump, "set autoscale x\n");  
    }

    if(fabs(min_value_y - max_value_y) > EPS){
        fprintf(gp_dump, "set yrange [%lg:%lg]\n", min_value_y, max_value_y);
    } 
    else{
        fprintf(gp_dump, "set autoscale y\n");  
    }
    if(is_taylor){
        fprintf(gp_dump, 
        "set key top left\n"          
        "set key box\n"               
        "set key spacing 2.5\n"
        "set key width 2.5\n"
        "set key height 1\n");  
    }
    

    fprintf(gp_dump, "\nf(%c) = ", forest->mtk->var_info[0].variable_name);
    CALL_FUNC_AND_CHECK_ERR(DumpToFileGp(gp_dump, forest->head_arr[idx1]->root, forest->mtk, err));
    fprintf(gp_dump, "\n");

    if(is_taylor){  
        fprintf(gp_dump, "Tf(%c) = ", forest->mtk->var_info[0].variable_name);
        CALL_FUNC_AND_CHECK_ERR(DumpToFileGp(gp_dump, forest->head_arr[idx2]->root, forest->mtk, err));
        fprintf(gp_dump, "\n");

        double result = 0;
        CALL_FUNC_AND_CHECK_ERR(CalcTreeExpression(forest->head_arr[1]->root, forest->mtk, &result, is_taylor, err));
        fprintf(gp_dump, 
            "x0 = %lg\n"          
            "y0 = f(x0)\n"            
            "k = %lg\n"
            "Tng(x) = (x - x0) * k + y0\n", forest->mtk->var_info[0].value, result);
    }

    fprintf(gp_dump, "plot f(%c) with lines linecolor \"blue\" title \"f(%c)\"", forest->mtk->var_info[0].variable_name, forest->mtk->var_info[0].variable_name);
    if(is_taylor){
        fprintf(gp_dump, " \\\n, Tf(%c) with lines linecolor \"red\" title \"T(%c)\"", forest->mtk->var_info[0].variable_name, forest->mtk->var_info[0].variable_name); // linewidth 2
        fprintf(gp_dump, " \\\n, Tng(%c) with lines linecolor \"green\" title \"Tng(%c)\" \n", forest->mtk->var_info[0].variable_name, forest->mtk->var_info[0].variable_name);
    }

}

static void MakePicture(const char* gp_filename, TreeErr_t* err){
    char sys_buffer[300] = "gnuplot ";

    strncat(sys_buffer, gp_filename, sizeof(sys_buffer) - 1);

    if(!is_cmd_for_sys_correct(sys_buffer, ALLOWED)){
        return;
    }

    if(system(sys_buffer)){;
        *err = CANT_MAKE_GRAPH;
    }
}