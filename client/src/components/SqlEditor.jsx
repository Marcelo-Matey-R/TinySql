function SqlEditor({ value, onChange, onExecute, isLoading }) {
    function handleKeyDown(e) {
        // Ejecutar con Ctrl+Enter
        if (e.ctrlKey && e.key === "Enter") {
            onExecute();
        }
    }

    return (
        <div className="flex flex-col gap-2">
            <textarea
                className="w-full h-48 bg-gray-800 text-green-400 font-mono
                           p-4 rounded border border-gray-600
                           focus:outline-none focus:border-blue-500
                           resize-none"
                value={value}
                onChange={e => onChange(e.target.value)}
                onKeyDown={handleKeyDown}
                placeholder="Escribí tu SQL acá...
-- Ejemplo:
-- CREATE DATABASE Universidad;
-- SET DATABASE Universidad;"
                spellCheck={false}
            />
            <button
                className="self-end bg-blue-600 hover:bg-blue-700
                           disabled:bg-gray-600 disabled:cursor-not-allowed
                           text-white px-6 py-2 rounded font-medium
                           transition-colors"
                onClick={onExecute}
                disabled={isLoading}
            >
                {isLoading ? "Ejecutando..." : "Ejecutar"}
            </button>
        </div>
    );
}

export default SqlEditor;