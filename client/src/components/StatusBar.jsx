function StatusBar({ currentDatabase, isLoading }) {
    return (
        <div className="flex items-center gap-4 px-4 py-2
                        bg-gray-800 border-b border-gray-700">
            {/* Logo / título */}
            <span className="text-blue-400 font-bold text-lg">
                TinySQLDb
            </span>

            <div className="w-px h-4 bg-gray-600" /> {/* separador */}

            {/* Base de datos activa */}
            <div className="flex items-center gap-2">
                <span className="text-gray-500 text-sm">Base de datos:</span>
                {currentDatabase ? (
                    <span className="text-green-400 text-sm font-mono">
                        {currentDatabase}
                    </span>
                ) : (
                    <span className="text-gray-600 text-sm italic">
                        ninguna
                    </span>
                )}
            </div>

            <div className="w-px h-4 bg-gray-600" /> {/* separador */}

            {/* Indicador de estado */}
            {isLoading ? (
                <span className="text-yellow-400 text-sm">
                    ● Procesando...
                </span>
            ) : (
                <span className="text-gray-600 text-sm">
                    ● Listo
                </span>
            )}
        </div>
    );
}

export default StatusBar;