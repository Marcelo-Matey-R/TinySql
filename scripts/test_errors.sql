-- ============================================
-- SCRIPT: CASOS DE ERROR
-- TinySQLDb - CE 2103
-- Todos estos casos deben retornar error
-- ============================================

-- ─── SETUP ───────────────────────────────────
CREATE DATABASE Universidad;
SET DATABASE Universidad;
CREATE TABLE Estudiante (
    ID INTEGER,
    Nombre VARCHAR(30),
    PrimerApellido VARCHAR(30)
);
INSERT INTO Estudiante (1, "Isaac", "Ramirez");
INSERT INTO Estudiante (2, "Juan", "Herrera");

-- ─── ERRORES DE BASE DE DATOS ────────────────

-- ERROR: Crear base de datos que ya existe
CREATE DATABASE Universidad;

-- ERROR: Usar base de datos que no existe
SET DATABASE BaseFalsa;

-- ─── ERRORES DE TABLA ────────────────────────

-- ERROR: Crear tabla que ya existe
CREATE TABLE Estudiante (ID INTEGER);

-- ERROR: DROP TABLE con datos adentro
DROP TABLE Estudiante;

-- ERROR: SELECT en tabla que no existe
SELECT * FROM TablaFalsa;

-- ERROR: INSERT en tabla que no existe
INSERT INTO TablaFalsa (1, "Test", "Test");

-- ERROR: UPDATE en tabla que no existe
UPDATE TablaFalsa SET Nombre = "Test";

-- ERROR: DELETE en tabla que no existe
DELETE FROM TablaFalsa;

-- ─── ERRORES DE COLUMNAS ─────────────────────

-- ERROR: SELECT con columna que no existe
SELECT ColumnaFalsa FROM Estudiante;

-- ERROR: WHERE con columna que no existe
SELECT * FROM Estudiante WHERE ColumnaFalsa = 1;

-- ERROR: UPDATE con columna que no existe
UPDATE Estudiante SET ColumnaFalsa = "Test";

-- ERROR: ORDER BY con columna que no existe
SELECT * FROM Estudiante ORDER BY ColumnaFalsa ASC;

-- ─── ERRORES DE TIPOS ────────────────────────

-- ERROR: INSERT con tipo incorrecto en INTEGER
INSERT INTO Estudiante ("texto", "Juan", "Herrera");

-- ERROR: INSERT con número incorrecto de valores (menos)
INSERT INTO Estudiante (3, "Pedro");

-- ERROR: INSERT con número incorrecto de valores (más)
INSERT INTO Estudiante (3, "Pedro", "Lopez", "Extra");

-- ERROR: Valor VARCHAR demasiado largo
INSERT INTO Estudiante (3, "NombreMuyLargoQueExcedeElLimiteDeCaracteresPermitido", "Lopez");

-- ─── ERRORES DE ÍNDICES ──────────────────────

-- Crear índice exitoso primero
CREATE INDEX Estudiante_Id ON Estudiante(ID) OF TYPE BTREE;

-- ERROR: Insertar duplicado en columna indexada
INSERT INTO Estudiante (1, "Duplicado", "Test");

-- ERROR: Crear índice que ya existe en la misma columna
CREATE INDEX Estudiante_Id2 ON Estudiante(ID) OF TYPE BST;

-- ERROR: Crear índice en columna con duplicados existentes
CREATE TABLE Cursos (ID INTEGER, Nombre VARCHAR(30));
INSERT INTO Cursos (1, "Matematica");
INSERT INTO Cursos (1, "Fisica");
CREATE INDEX Cursos_Id ON Cursos(ID) OF TYPE BTREE;

-- ─── ERRORES DE SINTAXIS ─────────────────────

-- ERROR: Sentencia SQL inválida
SELEC * FROM Estudiante;

-- ERROR: CREATE sin especificar DATABASE o TABLE
CREATE Estudiante;

-- ERROR: INSERT sin VALUES
INSERT INTO Estudiante;
