;; builra project settings.

(require 'cc-styles)

;; Style for builra C++ code.
(c-add-style "builra"
             '("gnu"
	       (fill-column . 80)
	       (c++-indent-level . 2)
	       (c-basic-offset . 2)
	       (indent-tabs-mode . Nothing)
	       (c-offsets-alist . ((arglist-intro . ++)
				   (innamespace . 0)
				   (member-init-intro . ++)))))

(provide 'builra-project-settings)
