import { useState } from "react";
import SqlEditor from "./components/SqlEditor";
import ResultTable from "./components/ResultTable";
import StatusBar from "./components/StatusBar";
import { executeQuery } from "./services/api";

function App() {
  const [sql, setSql] = useState("");
  const [results, setResults] = useState([]);
  const [isLoading, setIsLoading] = useState(false);
  const [currentDatabase, setCurrentDatabase] = useState("");

  async function handleExecute() {
    if (!sql.trim() || isLoading) return;

    setIsLoading(true);
    setResults([]);

    try {
      const data = await executeQuery(sql);
      setResults(data.results);
      updateCurrentDatabase(sql, data.results);
    } catch (error) {
      setResults([{
        success: false,
        error: error.message,
        timeMs: 0,
        columns: [],
        rows: []
      }]);
    } finally {
      setIsLoading(false);
    }
  }

  function updateCurrentDatabase(sql, results) {
    // Buscar si alguna sentencia fue SET DATABASE exitosa
    const statements = sql.split(";").map(s => s.trim()).filter(Boolean);
    statements.forEach((stmt, index) => {
      const upper = stmt.toUpperCase();
      if (upper.startsWith("SET DATABASE") &&
          results[index] && results[index].success) {
        // Extraer el nombre de la base de datos
        const parts = stmt.split(/\s+/);
        if (parts.length >= 3) {
          setCurrentDatabase(parts[2]);
        }
      }
    });
  }

  return (
      <div className="min-h-screen bg-gray-900 text-white flex flex-col">
        {/* Barra superior */}
        <StatusBar
            currentDatabase={currentDatabase}
            isLoading={isLoading}
        />

        {/* Contenido principal */}
        <div className="flex flex-col gap-6 p-6 max-w-6xl mx-auto w-full">
          {/* Editor SQL */}
          <div className="flex flex-col gap-2">
            <h2 className="text-gray-400 text-sm uppercase tracking-wider">
              Editor SQL
            </h2>
            <SqlEditor
                value={sql}
                onChange={setSql}
                onExecute={handleExecute}
                isLoading={isLoading}
            />
          </div>

          {/* Resultados */}
          {results.length > 0 && (
              <div className="flex flex-col gap-2">
                <h2 className="text-gray-400 text-sm uppercase tracking-wider">
                  Resultados
                </h2>
                <ResultTable results={results} />
              </div>
          )}
        </div>
      </div>
  );
}

export default App;