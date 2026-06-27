#!/bin/bash
# Genera scripts de benchmark con diferentes cantidades de registros
# Uso: ./generate_benchmark.sh

generate_script() {
    local count=$1
    local filename="scripts/benchmark_${count}.sql"
    
    echo "Generando $filename con $count registros..."
    
    cat > $filename << EOF
-- ============================================
-- BENCHMARK: $count REGISTROS
-- TinySQLDb - CE 2103
-- ============================================

CREATE DATABASE Benchmark${count};
SET DATABASE Benchmark${count};

CREATE TABLE Productos (
    ID INTEGER,
    Nombre VARCHAR(50),
    Precio DOUBLE,
    Categoria VARCHAR(30)
);

EOF

    # Generar los inserts
    for ((i=1; i<=count; i++)); do
        if ((i % 2 == 0)); then
            cat >> $filename << EOF
INSERT INTO Productos ($i, "Producto$(printf '%05d' $i)", "$(echo "scale=1; $i * 1.5" | bc)", "Categoria2");
EOF
        else
            cat >> $filename << EOF
INSERT INTO Productos ($i, "Producto$(printf '%05d' $i)", "$(echo "scale=1; $i * 1.5" | bc)", "Categoria1");
EOF
        fi
    done

    # Búsquedas sin índice — buscar el último registro (peor caso)
    cat >> $filename << EOF

-- ─── BÚSQUEDA SIN ÍNDICE (scan secuencial) ───
-- Buscar el último registro es el peor caso
SELECT * FROM Productos WHERE ID = $count;
SELECT * FROM Productos WHERE ID = $count;
SELECT * FROM Productos WHERE ID = $count;

-- ─── CREAR ÍNDICE BTREE ───────────────────────
CREATE INDEX Productos_Id ON Productos(ID) OF TYPE BTREE;

-- ─── BÚSQUEDA CON ÍNDICE BTREE ───────────────
SELECT * FROM Productos WHERE ID = $count;
SELECT * FROM Productos WHERE ID = $count;
SELECT * FROM Productos WHERE ID = $count;

-- ─── CREAR ÍNDICE BST ────────────────────────
-- Necesita tabla nueva porque solo se permite un índice por columna
EOF

    # Tabla para BST
    cat >> $filename << EOF
CREATE TABLE ProductosBST (
    ID INTEGER,
    Nombre VARCHAR(50),
    Precio DOUBLE,
    Categoria VARCHAR(30)
);

EOF

    for ((i=1; i<=count; i++)); do
        if ((i % 2 == 0)); then
            cat >> $filename << EOF
INSERT INTO ProductosBST ($i, "Producto$(printf '%05d' $i)", "$(echo "scale=1; $i * 1.5" | bc)", "Categoria2");
EOF
        else
            cat >> $filename << EOF
INSERT INTO ProductosBST ($i, "Producto$(printf '%05d' $i)", "$(echo "scale=1; $i * 1.5" | bc)", "Categoria1");
EOF
        fi
    done

    cat >> $filename << EOF

-- ─── BÚSQUEDA SIN ÍNDICE ─────────────────────
SELECT * FROM ProductosBST WHERE ID = $count;
SELECT * FROM ProductosBST WHERE ID = $count;
SELECT * FROM ProductosBST WHERE ID = $count;

-- ─── CREAR ÍNDICE BST ────────────────────────
CREATE INDEX ProductosBST_Id ON ProductosBST(ID) OF TYPE BST;

-- ─── BÚSQUEDA CON ÍNDICE BST ─────────────────
SELECT * FROM ProductosBST WHERE ID = $count;
SELECT * FROM ProductosBST WHERE ID = $count;
SELECT * FROM ProductosBST WHERE ID = $count;
EOF

    echo "✓ $filename generado con $count registros"
}

# Generar scripts para cada tamaño
generate_script 50
generate_script 500
generate_script 2500

echo ""
echo "Scripts generados en la carpeta scripts/"
echo "Ejecutalos en orden para ver la diferencia de rendimiento"
