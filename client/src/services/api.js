const API_URL = "http://localhost:8080";

export async function executeQuery(sql) {
    const response = await fetch(`${API_URL}/query`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ sql })
    });

    if (!response.ok)
        throw new Error(`Server error: ${response.status}`);

    return await response.json();
}