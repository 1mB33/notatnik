"use client";

import React from "react";

type Status = "loading" | "ready" | "missing" | "error";

export default function Note() {
    const [content, setContent] = React.useState("");
    const [status, setStatus] = React.useState<Status>("loading");
    const textareaRef = React.useRef<HTMLTextAreaElement>(null);

    const fit = React.useCallback(() => {
        const el = textareaRef.current;
        if (!el) return;
        el.style.height = "auto";
        el.style.height = `${el.scrollHeight + (el.offsetHeight - el.clientHeight)}px`;
    }, []);

    React.useLayoutEffect(fit, [content, status, fit]);

    React.useEffect(() => {
        window.addEventListener("resize", fit);
        return () => window.removeEventListener("resize", fit);
    }, [fit]);

    React.useEffect(() => {
        const controller = new AbortController();

        (async () => {
            try {
                const response = await fetch("/api/read", {
                    method: "POST",
                    headers: { "Content-Type": "application/json" },
                    body: JSON.stringify({ ownerId: 1, noteId: 81 }),
                    signal: controller.signal,
                });

                if (response.status === 404) return setStatus("missing");
                if (!response.ok) return setStatus("error");

                const data = await response.json();
                setContent(data?.noteContent ?? "");
                setStatus("ready");
            } catch (err) {
                if ((err as Error).name !== "AbortError") setStatus("error");
            }
        })();

        return () => controller.abort();
    }, []);

    function handleSave() {
        alert('clicked save :)');
    }

    if (status === "loading") return <span>...</span>;
    if (status === "missing") return <span>nie ma</span>;
    if (status === "error") return <span>błąd</span>;

    return (
        <div className="pt-10 pb-10">
            <textarea
                ref={textareaRef}
                rows={1}
                value={content}
                onChange={(e) => setContent(e.target.value)}
                className="box-border w-full resize-none overflow-hidden border-none outline-none"
            />
            <button className="pt-5 block" type="button" onClick={handleSave}>
                save
            </button>
        </div>
    );
}
