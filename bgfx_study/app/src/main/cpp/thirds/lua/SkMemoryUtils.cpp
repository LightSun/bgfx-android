//
// Created by Administrator on 2020/10/27 0027.
//

#include "SkMemoryUtils.h"
#include "SkMemory.h"
#include "log.h"

void SkMemoryUtils::reshapeMat(SkMemoryMatrix *mat, const char* mode,int rowC, int colC, char t,
                                      double defVal, SkHolder *out) {
    if(t == DEF_RESHAPE_TYPE){
        t = MemoryUtils::computeType(mat->getTypes());
    }
    //compute a correct row-count or column count
    if(colC == -1){
        if(rowC == -1){
            out->type = TYPE_ERROR_MSG;
            out->msg = "row-count and column-count can't be -1 at the same";
            return;
        }
        auto total = mat->getRowCount() * mat->getColumnCount();
        colC = total / rowC + ((total % rowC) > 0 ? 1 : 0);
    } else if(rowC == -1){
        if(colC == 0){
            out->type = TYPE_ERROR_MSG;
            out->msg = "row-count and column-count must be valid.";
            return;
        }
        auto total = mat->getRowCount() * mat->getColumnCount();
        rowC = total / colC + ((total % colC) > 0 ? 1 : 0);
    }
    if(strcmp(mode, MODE_ROW) == 0){
        if(colC == 0){
            SkASSERT(rowC > 0);
            //only mem-data.
            auto outMem = SkMemory::create(t, rowC);
            auto unitSize = MemoryUtils::getUnitSize(t);
            double val;
            for (int i = 0; i < rowC; ++i) {
                if(!mat->getValueRowByRow(i, &val)){
                    val = defVal;
                }
                MemoryUtils::setValue(outMem->data, t, i * unitSize, val);
            }
            out->ptr = outMem;
            out->type = TYPE_MEM;
        } else{
            SkASSERT(rowC > 0);
            SkASSERT(colC > 0);
            const auto outMat = new SkMemoryMatrix(rowC, true);
            //init mat
            for (int i = 0; i < rowC; ++i) {
                outMat->array[i] = SkMemory::create(t, colC);
            }
            //set value
            double val;
            int index = 0;
            for (int i = 0; i < rowC; ++i) {
                for (int j = 0; j < colC; ++j) {
                    if(!mat->getValueRowByRow(index++, &val)){
                        val = defVal;
                    }
                    outMat->setValue(i, j, val);
                }
            }
            out->ptr = outMat;
            out->type = TYPE_MEM_MAT;
        }
    } else if(strcmp(mode, MODE_COLUMN) == 0){
        if(colC == 0){
            SkASSERT(rowC > 0);
            //only mem-data.
            auto outMem = SkMemory::create(t, rowC);
            auto unitSize = MemoryUtils::getUnitSize(t);
            double val;
            for (int i = 0; i < rowC; ++i) {
                if(!mat->getValueColByCol(i, &val)){
                    val = defVal;
                }
                MemoryUtils::setValue(outMem->data, t, i * unitSize, val);
            }
            out->ptr = outMem;
            out->type = TYPE_MEM;
        } else{
            SkASSERT(rowC > 0);
            SkASSERT(colC > 0);
            const auto outMat = new SkMemoryMatrix(rowC, true);
            //init mat
            for (int i = 0; i < rowC; ++i) {
                outMat->array[i] = SkMemory::create(t, colC);
            }
            //set value
            double val;
            int index = 0;
            for (int i = 0; i < rowC; ++i) {
                for (int j = 0; j < colC; ++j) {
                    if(!mat->getValueColByCol(index++, &val)){
                        val = defVal;
                    }
                    outMat->setValue(i, j, val);
                }
            }
            out->ptr = outMat;
            out->type = TYPE_MEM_MAT;
        }
    } else if(strcmp(mode, MODE_ROW_COLUMN) == 0){
        out->ptr = mat->reshape(rowC, colC, t, defVal);
        out->type = TYPE_MEM_MAT;
    }
    else{
        LOGW("unknown mode = %s", mode);
        out->type = TYPE_ERROR_MSG;
        out->msg = "unknown mode";
    }
}

void SkMemoryUtils::reshapeMem(SkMemory *mem, int rowC, int colC, char t, double defVal,
                                      SkHolder *out) {
    if(t == DEF_RESHAPE_TYPE){
        t = mem->getTypes()[0];
    }
    if(colC == -1){
        if(rowC == -1){
            out->type = TYPE_ERROR_MSG;
            out->msg = "row-count and column-count can't be -1 at the same";
            return;
        }
        auto total = mem->getLength();
        colC = total / rowC + ((total % rowC) > 0 ? 1 : 0);
    } else if(rowC == -1){
        if(colC == 0){
            out->type = TYPE_ERROR_MSG;
            out->msg = "row-count and column-count must be valid.";
            return;
        }
        auto total = mem->getLength();
        rowC = total / colC + ((total % colC) > 0 ? 1 : 0);
    }
    if(colC == 0){
        //only mem
        auto outMem = SkMemory::create(t, rowC);
        auto unitSize = MemoryUtils::getUnitSize(t);
        double val;
        for (int i = 0; i < rowC; ++i) {
            if(!mem->getValue(i, &val)){
                val = defVal;
            }
            MemoryUtils::setValue(outMem->data, t, i * unitSize, val);
        }
        out->ptr = outMem;
        out->type = TYPE_MEM;
    } else{
        //compute a correct row-count or column count
        auto mat = new SkMemoryMatrix(rowC, true);
        for (int i = 0; i < rowC; ++i) {
            mat->array[i] = SkMemory::create(t, colC);
        }
        //set value
        double val;
        int index = 0;
        for (int i = 0; i < rowC; ++i) {
            for (int j = 0; j < colC; ++j) {
                if(!mem->getValue(index++, &val)){
                    val = defVal;
                }
                mat->setValue(i, j, val);
            }
        }
        out->ptr = mat;
        out->type = TYPE_MEM_MAT;
    }
}

void SkMemoryUtils::reshapeMem(SkAnyMemory *mem, int rowC, int colC, char t, double defVal,
                               SkHolder *out) {
    if(t == DEF_RESHAPE_TYPE){
        t = MemoryUtils::computeType(mem->getTypes());
    }
    if(colC == -1){
        if(rowC == -1){
            out->type = TYPE_ERROR_MSG;
            out->msg = "row-count and column-count can't be -1 at the same";
            return;
        }
        auto total = mem->getLength();
        colC = total / rowC + ((total % rowC) > 0 ? 1 : 0);
    } else if(rowC == -1){
        if(colC == 0){
            out->type = TYPE_ERROR_MSG;
            out->msg = "row-count and column-count must be valid.";
            return;
        }
        auto total = mem->getLength();
        rowC = total / colC + ((total % colC) > 0 ? 1 : 0);
    }
    if(colC == 0){
        //only mem
        auto outMem = SkMemory::create(t, rowC);
        auto unitSize = MemoryUtils::getUnitSize(t);
        double val;
        for (int i = 0; i < rowC; ++i) {
            if(!mem->getValue(i, &val)){
                val = defVal;
            }
            MemoryUtils::setValue(outMem->data, t, i * unitSize, val);
        }
        out->ptr = outMem;
        out->type = TYPE_MEM;
    } else{
        auto mat = new SkMemoryMatrix(rowC, true);
        for (int i = 0; i < rowC; ++i) {
            mat->array[i] = SkMemory::create(t, colC);
        }
        //set value
        double val;
        int index = 0;
        for (int i = 0; i < rowC; ++i) {
            for (int j = 0; j < colC; ++j) {
                if(!mem->getValue(index++, &val)){
                    val = defVal;
                }
                mat->setValue(i, j, val);
            }
        }
        out->ptr = mat;
        out->type = TYPE_MEM_MAT;
    }
}