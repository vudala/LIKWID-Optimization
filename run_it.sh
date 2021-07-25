PROGRAM=./matmult

# inicializa os arquivos auxiliares 
TMP=( rowVet_aux1.tmp matRow_aux1.tmp )
echo "matRowVet" > rowVet_aux1.tmp
echo "multMatMatRow" > matRow_aux1.tmp

for file in ${TMP[*]};
do
    echo "N:Memory bandwidth [MBytes/s]:Memory bandwidth [MBytes/s] (optimized)" >> $file
done

TMP2=( rowVet_aux2.tmp matRow_aux2.tmp )
for file in ${TMP2[*]};
do
    echo '----------------' > $file
    echo "N:Time elapsed [s]:Time elapsed [s] (optimized)" >> $file
done

# monitora a banda de memória das funções
for N in 64 100 128 1000 1024 2000 2048 3000 4096 5000;
do
    likwid-perfctr -C 3 -g L3 -m "$PROGRAM -n $N" > aux.tmp
    RESULT=$(cat aux.tmp | grep "L3 bandwidth" | cut -d'|' -f3)
    RESULT=($RESULT)
    
    echo -n "$N:" >> ${TMP[0]}
    echo "${RESULT[0]}:${RESULT[1]}" >> ${TMP[0]}
    
    echo -n "$N:" >> ${TMP[1]}
    echo "${RESULT[2]}:${RESULT[3]}" >> ${TMP[1]}

    # captura o tempo de execução das funções para ser escrito posteriormente
    RESULT=$(cat aux.tmp | grep "RDTSC Runtime" | cut -d'|' -f3)
    RESULT=($RESULT)

    echo -n "$N:" >> ${TMP2[0]}
    echo "${RESULT[0]}:${RESULT[1]}" >> ${TMP2[0]}
    
    echo -n "$N:" >> ${TMP2[1]}
    echo "${RESULT[2]}:${RESULT[3]}" >> ${TMP2[1]}
done

# concatena os arquivos auxiliares
for v in 0 1;
do
    cat ${TMP2[$v]} >> ${TMP[$v]}
done
rm ${TMP2[*]}

# monitora a taxa de cache miss das funções
for file in ${TMP[*]};
do
    echo '----------------' >> $file
    echo "N:Data cache miss ratio:Data cache miss ratio (optimized)" >> $file
done
for N in 64 100 128 1000 1024 2000 2048 3000 4096 5000;
do
    likwid-perfctr -C 3 -g L2CACHE -m $PROGRAM -n $N > aux.tmp
    RESULT=$(cat aux.tmp | grep "L2 miss ratio" | cut -d'|' -f3)
    RESULT=($RESULT)
    
    echo -n "$N:" >> ${TMP[0]}
    echo "${RESULT[0]}:${RESULT[1]}" >> ${TMP[0]}
    
    echo -n "$N:" >> ${TMP[1]}
    echo "${RESULT[2]}:${RESULT[3]}" >> ${TMP[1]}
done


# monitora os MFLOPS/s de cada uma das funções
for file in ${TMP[*]};
do  
    echo '----------------' >> $file
    echo "N:DP MFLOP/s:AVX DP MFLOP/s:DP MFLOP/s (optimized):AVX DP MFLOP/s (optimized)" >> $file
done
for N in 64 100 128 1000 1024 2000 2048 3000 4096 5000;
do
    likwid-perfctr -C 3 -g FLOPS_DP -m "$PROGRAM -n $N" > aux.tmp
    RESULT_DP=$(cat aux.tmp | grep -v "AVX" | grep "DP MFLOP/s" | cut -d'|' -f3)
    RESULT_DP=($RESULT_DP)

    RESULT_AVX=$(cat aux.tmp | grep "AVX" | cut -d'|' -f3)
    RESULT_AVX=($RESULT_AVX)

    echo -n "$N:" >> ${TMP[0]}
    echo "${RESULT_DP[0]}:${RESULT_AVX[0]}:${RESULT_DP[1]}:${RESULT_AVX[1]}" >> ${TMP[0]}
    
    echo -n "$N:" >> ${TMP[1]}
    echo "${RESULT_DP[2]}:${RESULT_AVX[2]}:${RESULT_DP[3]}:${RESULT_AVX[3]}" >> ${TMP[1]}
done

# deixa os arquivos bonitinhos pra apresentação
for file in ${TMP[*]};
do
    column -s : -t $file
    rm $file
    echo
done

rm aux.tmp

