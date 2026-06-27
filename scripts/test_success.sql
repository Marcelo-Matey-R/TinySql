-- ============================================
-- SCRIPT: CASOS DE ÉXITO
-- TinySQLDb - CE 2103
-- ============================================

-- ─── SETUP ───────────────────────────────────
CREATE DATABASE Universidad;
SET DATABASE Universidad;

-- ─── CREATE TABLE ────────────────────────────
CREATE TABLE Estudiante (
    ID INTEGER,
    Nombre VARCHAR(30),
    PrimerApellido VARCHAR(30),
    SegundoApellido VARCHAR(30),
    FechaNacimiento DATETIME
);

-- ─── INSERT ──────────────────────────────────
INSERT INTO Estudiante (1, "Isaac", "Ramirez", "Herrera", "2000-01-01 01:02:00");
INSERT INTO Estudiante (2, "Juan", "Ramirez", "Mora", "1999-05-15 00:00:00");
INSERT INTO Estudiante (3, "Pedro", "Herrera", "Lopez", "2001-03-20 00:00:00");
INSERT INTO Estudiante (4, "Maria", "Lopez", "Jimenez", "2000-08-10 00:00:00");
INSERT INTO Estudiante (5, "Ana", "Mora", "Vargas", "1998-12-01 00:00:00");

-- ─── SELECT ──────────────────────────────────

-- SELECT todos los registros
SELECT * FROM Estudiante;

-- SELECT columnas específicas
SELECT Nombre, PrimerApellido FROM Estudiante;

-- SELECT con WHERE igual
SELECT * FROM Estudiante WHERE ID = 3;

-- SELECT con WHERE mayor
SELECT * FROM Estudiante WHERE ID > 2;

-- SELECT con WHERE menor
SELECT * FROM Estudiante WHERE ID < 3;

-- SELECT con LIKE
SELECT * FROM Estudiante WHERE PrimerApellido LIKE *mire*;

-- SELECT con ORDER BY ASC
SELECT * FROM Estudiante ORDER BY Nombre ASC;

-- SELECT con ORDER BY DESC
SELECT * FROM Estudiante ORDER BY Nombre DESC;

-- SELECT con WHERE y ORDER BY combinados
SELECT * FROM Estudiante WHERE ID > 2 ORDER BY Nombre ASC;

-- ─── UPDATE ──────────────────────────────────

-- UPDATE con WHERE
UPDATE Estudiante SET Nombre = "Felipe" WHERE ID = 1;

-- Verificar UPDATE
SELECT * FROM Estudiante WHERE ID = 1;

-- UPDATE sin WHERE (actualiza todos)
UPDATE Estudiante SET SegundoApellido = "Actualizado";

-- Verificar UPDATE masivo
SELECT * FROM Estudiante;

-- ─── DELETE ──────────────────────────────────

-- DELETE con WHERE
DELETE FROM Estudiante WHERE ID = 5;

-- Verificar DELETE
SELECT * FROM Estudiante;

-- ─── ÍNDICES ─────────────────────────────────

-- Crear índice BTREE
CREATE INDEX Estudiante_Id ON Estudiante(ID) OF TYPE BTREE;

-- SELECT usando índice BTREE
SELECT * FROM Estudiante WHERE ID = 3;

-- ─── DROP TABLE ──────────────────────────────

-- Crear tabla vacía para poder eliminarla
CREATE TABLE Temporal (ID INTEGER, Nombre VARCHAR(30));

-- DROP exitoso sobre tabla vacía
DROP TABLE Temporal;

-- ─── SYSTEM CATALOG ──────────────────────────

-- Consultar bases de datos existentes
SELECT * FROM SystemDatabases;

-- Consultar tablas existentes
SELECT * FROM SystemTables;

-- Consultar columnas existentes
SELECT * FROM SystemColumns;

-- Consultar índices existentes
SELECT * FROM SystemIndexes;
