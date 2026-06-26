function ResultTable({ results }) {
    if (!results || results.length === 0) return null;

    return (
        <div className="flex flex-col gap-6">
            {results.map((result, index) => (
                <div key={index} className="flex flex-col gap-2">
                    {/* Tiempo de ejecución */}
                    <p className="text-gray-400 text-sm">
                        Sentencia {index + 1} — {result.timeMs}ms
                    </p>

                    {/* Error */}
                    {!result.success && (
                        <div className="bg-red-900 border border-red-500
                                        text-red-200 p-3 rounded">
                            {result.error}
                        </div>
                    )}

                    {/* Tabla de resultados */}
                    {result.success && result.rows && result.rows.length > 0 && (
                        <div className="overflow-x-auto">
                            <table className="w-full border-collapse">
                                <thead>
                                <tr className="bg-gray-700">
                                    {result.columns.map((col, i) => (
                                        <th key={i}
                                            className="text-left text-white
                                                           px-4 py-2 border
                                                           border-gray-600">
                                            {col}
                                        </th>
                                    ))}
                                </tr>
                                </thead>
                                <tbody>
                                {result.rows.map((row, rowIndex) => (
                                    <tr key={rowIndex}
                                        className="hover:bg-gray-700
                                                       transition-colors">
                                        {row.map((cell, cellIndex) => (
                                            <td key={cellIndex}
                                                className="text-gray-300
                                                               px-4 py-2 border
                                                               border-gray-600">
                                                {cell}
                                            </td>
                                        ))}
                                    </tr>
                                ))}
                                </tbody>
                            </table>
                        </div>
                    )}

                    {/* SELECT exitoso pero sin resultados */}
                    {result.success && result.columns &&
                        result.columns.length > 0 &&
                        result.rows.length === 0 && (
                            <p className="text-gray-500 italic">
                                Sin resultados
                            </p>
                        )}

                    {/* Operación exitosa sin filas (INSERT, UPDATE, etc) */}
                    {result.success &&
                        (!result.columns || result.columns.length === 0) && (
                            <p className="text-green-400">
                                ✓ Operación exitosa
                            </p>
                        )}
                </div>
            ))}
        </div>
    );
}

export default ResultTable;