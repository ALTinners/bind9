;;; Directory Local Variables
;;; For more information see (info "(emacs) Directory Variables")

((c-mode .
  ((eval .
	 (set (make-local-variable 'directory-of-current-dir-locals-file)
	      (file-name-directory (locate-dominating-file default-directory ".dir-locals.el"))
	      )
	 )
   (eval .
	 (set (make-local-variable 'include-directories)
	      (list

	       ;; top directory
	       (expand-file-name
		(concat directory-of-current-dir-locals-file "./"))

	       ;; current directory
	       (expand-file-name (concat default-directory "./"))

	       ;; libisc
	       (expand-file-name
		(concat directory-of-current-dir-locals-file "lib/isc/unix/include"))
	       (expand-file-name
		(concat directory-of-current-dir-locals-file "lib/isc/pthreads/include"))
	       (expand-file-name
		(concat directory-of-current-dir-locals-file "lib/isc/include"))

	       ;; libdns

	       (expand-file-name
		(concat directory-of-current-dir-locals-file "lib/dns/include"))

	       ;; libisccc
	       (expand-file-name
		(concat directory-of-current-dir-locals-file "lib/isccc/include"))

	       ;; libisccfg
	       (expand-file-name
		(concat directory-of-current-dir-locals-file "lib/isccfg/include"))

	       ;; libns
	       (expand-file-name
		(concat directory-of-current-dir-locals-file "lib/ns/include"))

	       ;; libirs
	       (expand-file-name
		(concat directory-of-current-dir-locals-file "lib/irs/include"))

	       ;; libbind9
	       (expand-file-name
		(concat directory-of-current-dir-locals-file "lib/bind9/include"))

	       (expand-file-name "/usr/local/opt/openssl@1.1/include")
	       (expand-file-name "/usr/local/opt/libxml2/include/libxml2")
	       (expand-file-name "/usr/local/opt/json-c/include/json-c/")
	       (expand-file-name "/usr/local/include")
	       )
	      )
	 )

   (eval setq flycheck-clang-include-path include-directories)
   (eval setq flycheck-cppcheck-include-path include-directories)
   (eval setq flycheck-clang-args
	 (list
	  "-include"
	  (expand-file-name
	   (concat directory-of-current-dir-locals-file "config.h"))
	  )
	 )
   )
  ))
